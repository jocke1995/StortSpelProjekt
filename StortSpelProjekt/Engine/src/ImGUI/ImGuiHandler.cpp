#include "ImGuiHandler.h"
#include "../Misc/Option.h"
#include "../Events/EventBus.h"

ImGuiHandler& ImGuiHandler::GetInstance()
{
	static ImGuiHandler instance;

	return instance;
}

void ImGuiHandler::NewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiHandler::UpdateFrame()
{
    DrawConsole("Console");
}

void ImGuiHandler::SetFloat(std::string name, float value)
{
    m_FloatMap[name] = value;
}

void ImGuiHandler::SetVec4(std::string name, ImVec4 value)
{
    m_Vec4Map[name] = value;
}

void ImGuiHandler::SetFloat4(std::string name, float4 value)
{
    m_Vec4Map[name] = ImVec4(value.x, value.y, value.z, value.w);
}

void ImGuiHandler::SetXMFLOAT4(std::string name, DirectX::XMFLOAT4 value)
{
    m_Vec4Map[name] = ImVec4(value.x, value.y, value.z, value.w);
}

float ImGuiHandler::GetFloat(std::string name)
{
    if (m_FloatMap.find(name) == m_FloatMap.end())
    {
        Log::PrintSeverity(Log::Severity::WARNING, "ImGui variable %s does not exist\n", name);
        return 0.0f;
    }
    else
    {
        return m_FloatMap[name];
    }
}

ImVec4 ImGuiHandler::GetVec4(std::string name)
{
    if (m_Vec4Map.find(name) == m_Vec4Map.end())
    {
        Log::PrintSeverity(Log::Severity::WARNING, "ImGui variable %s does not exist\n", name);
        return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        return m_Vec4Map[name];
    }
}

float4 ImGuiHandler::GetFloat4(std::string name)
{
    if (m_Vec4Map.find(name) == m_Vec4Map.end())
    {
        Log::PrintSeverity(Log::Severity::WARNING, "ImGui variable %s does not exist\n", name);
        return float4({ 0.0f, 0.0f, 0.0f, 0.0f });
    }
    else
    {
        return float4({ m_Vec4Map[name].x, m_Vec4Map[name].y, m_Vec4Map[name].z, m_Vec4Map[name].w });
    }
}

DirectX::XMFLOAT4 ImGuiHandler::GetXMFLOAT4(std::string name)
{
    if (m_Vec4Map.find(name) == m_Vec4Map.end())
    {
        Log::PrintSeverity(Log::Severity::WARNING, "ImGui variable %s does not exist\n", name);
        return DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
    }
    else
    {
        return DirectX::XMFLOAT4( m_Vec4Map[name].x, m_Vec4Map[name].y, m_Vec4Map[name].z, m_Vec4Map[name].w );
    }
}

void ImGuiHandler::ClearLog()
{
    for (int i = 0; i < m_Items.Size; ++i)
    {
        free(m_Items[i]);
    }
    m_Items.clear();
}

void ImGuiHandler::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    m_Items.push_back(Strdup(buf));
}

void ImGuiHandler::DrawConsole(const char* title)
{
    ImGui::SetNextWindowSize(ImVec2(Option::GetInstance().GetVariable("windowWidth") / 2, Option::GetInstance().GetVariable("windowHeight")));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(title))
    {
        ImGui::End();
        return;
    }

    ImGui::TextWrapped("Enter 'HELP' for help.");

    if (ImGui::SmallButton("Clear"))
    {
        ClearLog();
    }

    ImGui::Separator();

    m_Filter.Draw("Filter (\"incl,-excl\")", 180);
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("Clear"))
        {
            ClearLog();
        }
        ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    for (int i = 0; i < m_Items.Size; ++i)
    {
        const char* item = m_Items[i];
        if (!m_Filter.PassFilter(item))
        {
            continue;
        }

        // Normally you would store more information in your item than just a string.
        // (e.g. make Items[] an array of structure, store color/type etc.)
        ImVec4 color;
        bool has_color = false;
        if (strstr(item, "[error]"))
        {
            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true;
        }
        else if (strncmp(item, "# ", 2) == 0)
        {
            color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true;
        }

        if (has_color)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        }
        ImGui::TextUnformatted(item);
        if (has_color)
        {
            ImGui::PopStyleColor();
        }
    }

    if (m_ScrollToBottom)
    {
        ImGui::SetScrollHereY(1.0f);
    }
    m_ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", m_InputBuf, IM_ARRAYSIZE(m_InputBuf), input_text_flags, &TextEditCallbackStub, static_cast<void*>(this)))
    {
        char* s = m_InputBuf;
        Strtrim(s);
        if (s[0])
        {
            ExecCommand(s);
        }
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
    {
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    ImGui::End();
}

void ImGuiHandler::ExecCommand(const char* command_line)
{
    AddLog("# %s\n", command_line);

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    m_HistoryPos = -1;
    for (int i = m_History.Size - 1; i >= 0; --i)
        if (Stricmp(m_History[i], command_line) == 0)
        {
            free(m_History[i]);
            m_History.erase(m_History.begin() + i);
            break;
        }
    m_History.push_back(Strdup(command_line));

    char command[256];
    for (int i = 0; i < 256; ++i)
    {
        if (!command_line[i])
        {
            break;
        }
        else if (command_line[i] == ':')
        {

        }
        command[i] = command_line[i];
    }

    // Process command
    if (Stricmp(command_line, "CLEAR") == 0)
    {
        ClearLog();
    }
    else if (Stricmp(command_line, "HELP") == 0)
    {
        AddLog("Commands:");
        for (int i = 0; i < m_Commands.Size; ++i)
        {
            AddLog("- %s", m_Commands[i]);
        }
    }
    else if (Stricmp(command_line, "HISTORY") == 0)
    {
        int first = m_History.Size - 10;
        for (int i = first > 0 ? first : 0; i < m_History.Size; ++i)
        {
            AddLog("%3d: %s\n", i, m_History[i]);
        }
    }
    else if (Stricmp(command_line, "UNLOCK CAMERA") == 0)
    {
        EventBus::GetInstance().Publish(&ModifierInput(SCAN_CODES::LEFT_CTRL, true));
    }
    else
    {
        AddLog("Unknown command: '%s'\n", command_line);
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    m_ScrollToBottom = true;
}

int ImGuiHandler::TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    ImGuiHandler* console = reinterpret_cast<ImGuiHandler*>(data->UserData);
    return console->TextEditCallback(data);
}

int ImGuiHandler::TextEditCallback(ImGuiInputTextCallbackData* data)
{
    //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                {
                    break;
                }
                --word_start;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < m_Commands.Size; ++i)
            {
                if (Strnicmp(m_Commands[i], word_start, static_cast<int>(word_end - word_start)) == 0)
                {
                    candidates.push_back(m_Commands[i]);
                }
            }

            if (candidates.Size == 0)
            {
                // No match
                AddLog("No match for \"%.*s\"!\n", static_cast<int>(word_end - word_start), word_start);
            }
            else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                data->DeleteChars((int)(word_start - data->Buf), static_cast<int>(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                // Multiple matches. Complete as much as we can..
                // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                int match_len = static_cast<int>(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; ++i)
                    {
                        if (i == 0)
                        {
                            c = toupper(candidates[i][match_len]);
                        }
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                        {
                            all_candidates_matches = false;
                        }
                    }
                    if (!all_candidates_matches)
                    {
                        break;
                    }
                    ++match_len;
                }

                if (match_len > 0)
                {
                    data->DeleteChars(static_cast<int>(word_start - data->Buf), static_cast<int>(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; ++i)
                {
                    AddLog("- %s\n", candidates[i]);
                }
            }

            break;
        }
    case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = m_HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (m_HistoryPos == -1)
                {
                    m_HistoryPos = m_History.Size - 1;
                }
                else if (m_HistoryPos > 0)
                {
                    --m_HistoryPos;
                }
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (m_HistoryPos != -1)
                {
                    if (++m_HistoryPos >= m_History.Size)
                    {
                        m_HistoryPos = -1;
                    }
                }
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != m_HistoryPos)
            {
                const char* history_str = (m_HistoryPos >= 0) ? m_History[m_HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
    }
    return 0;
}

ImGuiHandler::ImGuiHandler()
{
    m_TextMap["console"] = " ";

    ClearLog();
    memset(m_InputBuf, 0, sizeof(m_InputBuf));
    m_HistoryPos = -1;

    m_Commands.push_back("HELP");
    m_Commands.push_back("HISTORY");
    m_Commands.push_back("CLEAR");
    m_Commands.push_back("UNLOCK CAMERA");
    m_ScrollToBottom = false;
    AddLog("Console initiated!");
}

ImGuiHandler::~ImGuiHandler()
{
    ClearLog();
    for (int i = 0; i < m_History.Size; ++i)
    {
        free(m_History[i]);
    }
}
