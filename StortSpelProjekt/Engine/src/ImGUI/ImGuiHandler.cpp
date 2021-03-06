#include "ImGuiHandler.h"
#include "../Misc/Option.h"
#include "../Misc/Window.h"
#include "../Renderer/Renderer.h"
#include "../Events/EventBus.h"
#include "../../Game/src/Gamefiles/Player.h"
#include "../ECS/Entity.h"
#include "../../Game/src/Gamefiles/EnemyFactory.h"

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
    Renderer& r = Renderer::GetInstance();
    const Window* window = r.GetWindow();
    

    // Set the size and position of the debug info window and set it to start not collapsed. m_NumberOfDebuggingLines is set in constructor
    ImGui::SetNextWindowSize(ImVec2(window->GetScreenWidth() / 2, ImGui::GetTextLineHeightWithSpacing() * (2 + m_NumberOfDebuggingLines)));
    ImGui::SetNextWindowPos(ImVec2(window->GetScreenWidth() / 2, 0));
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_Appearing);

    // Initiate the debug info window
    ImGui::Begin("Debugging info");
    // This is where debug info should be added
#pragma region debugInfo
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
    {
        ImGui::Text("Bounding boxes are turned %s", m_BoolMap["boundingBoxToggle"] ? "on" : "off");
    }
    // Vram usage
    DXGI_QUERY_VIDEO_MEMORY_INFO info;
    if (SUCCEEDED(r.m_pAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
    {
        float memoryUsage = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB
        ImGui::Text("VRAM usage: %.02f MiB", memoryUsage);
    };

    // Ram usage
    PROCESS_MEMORY_COUNTERS pmc{};
    if (GetProcessMemoryInfo(r.m_ProcessHandle, &pmc, sizeof(pmc)))
    {
        //PagefileUsage is the:
            //The Commit Charge value in bytes for this process.
            //Commit Charge is the total amount of memory that the memory manager has committed for a running process.
        float memoryUsage = float(pmc.WorkingSetSize / 1024.0 / 1024.0); //MiB
        ImGui::Text("RAM usage: %.02f MiB", memoryUsage);
    }

    // Adapter
    DXGI_ADAPTER_DESC3 desc = {};
    r.m_pAdapter4->GetDesc3(&desc);
    ImGui::Text("Adapter: %S", desc.Description);

#pragma endregion debugInfo
    ImGui::End();

    // Initiate the debug info window
    ImGui::Begin("Enemy info");
    // This is where debug info should be added
#pragma region enemyInfo
    ImGui::Text("Difficulty Scaler: %.1f", EnemyFactory::GetInstance().GetDifficultyScaler());
    ImGui::Text("Zombie:");
    EnemyComps* zombie = EnemyFactory::GetInstance().GetDefineEnemy("enemyZombie");
    if (zombie != nullptr)
    {
        ImGui::Text("\tHealth: %d", zombie->hp);
        ImGui::Text("\tDamage: %.1f", zombie->meleeAttackDmg);
    }
    ImGui::Text("Spider:");
    EnemyComps* spider = EnemyFactory::GetInstance().GetDefineEnemy("enemySpider");
    if (spider != nullptr)
    {
        ImGui::Text("\tHealth: %d", spider->hp);
        ImGui::Text("\tDamage: %.1f", spider->meleeAttackDmg);
    }
    ImGui::Text("Demon:");
    EnemyComps* demon = EnemyFactory::GetInstance().GetDefineEnemy("enemyDemon");
    if (demon != nullptr)
    {
        ImGui::Text("\tHealth: %d", demon->hp);
        ImGui::Text("\tDamage: %.1f", demon->rangeAttackDmg);
    }
#pragma endregion enemyInfo
    ImGui::End();

    DrawConsole("Console");
}

void ImGuiHandler::SetBool(std::string name, bool value)
{
    m_BoolMap[name] = value;
}

bool ImGuiHandler::GetBool(std::string name)
{
    bool toReturn = m_BoolMap.find(name) == m_BoolMap.end() ? false : m_BoolMap[name];
    return toReturn;
}

int ImGuiHandler::Stricmp(const char* s1, const char* s2)
{
    int d;
    while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
    {
        s1++;
        s2++;
    }
    return d;
}

int ImGuiHandler::Strnicmp(const char* s1, const char* s2, int n)
{
    int d = 0;
    while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
    {
        s1++;
        s2++;
        n--;
    }
    return d;
}

char* ImGuiHandler::Strdup(const char* s)
{
    size_t len = strlen(s) + 1;
    void* buf = malloc(len);
    IM_ASSERT(buf);
    return static_cast<char*>(memcpy(buf, static_cast<const void*>(s), len));
}

void ImGuiHandler::Strtrim(char* s)
{
    char* str_end = s + strlen(s);
    while (str_end > s && str_end[-1] == ' ')
    {
        str_end--;
        *str_end = 0;
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
	const Window* window = Renderer::GetInstance().GetWindow();

    // Set the size and position of the console window and set it to start collapsed
    ImGui::SetNextWindowSize(ImVec2(window->GetScreenWidth() / 2, window->GetScreenHeight()));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);
    if (!ImGui::Begin(title))
    {
        ImGui::End();
        return;
    }

    ImGui::TextWrapped("To use:\nEnter a valid command and press ENTER. The console is not case sensitive.\nIf the command takes arguments, enter these after \':\'\nSeparate arguments using \',\'\nExample: COMMAND:ARG1,ARG2\nEnter 'HELP' for help.");

    // Create button to clear the console
    if (ImGui::SmallButton("Clear"))
    {
        ClearLog();
    }

    ImGui::Separator();

    // Add a text filter
    m_Filter.Draw("Filter (\"incl,-excl\")", 180);
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

    // Create the console
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

    // Add context window on right click
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

    std::string command_line_string(command_line);

    // Divide the entry into command and arguments
    INT64 dividerPos = command_line_string.find(':');
    std::string command = command_line_string.substr(0, dividerPos);
    INT64 dividerOffset = 1;

    // Ignore spaces after divider
    while (command_line_string[dividerPos + dividerOffset] == ' ')
    {
        ++dividerOffset;
    }
    std::string argumentList = command_line_string.substr(dividerPos + dividerOffset);

    // If there are no arguments, set argument to -1
    if (dividerPos == -1 || argumentList == "")
    {
        argumentList = "-1";
    }

    // Divide the argument list and store in vector
    std::vector<std::string> arguments;
    INT64 prevDivider = -1;
    dividerPos = argumentList.find(',');

    while (dividerPos != -1)
    {
        arguments.push_back(argumentList.substr(prevDivider + 1, dividerPos - prevDivider - 1));
        prevDivider = dividerPos;
        dividerPos = argumentList.find(',', dividerPos + 1);
    }
    if (argumentList.substr(prevDivider + 1) != "")
    {
        arguments.push_back(argumentList.substr(prevDivider + 1));
    }

    // Process command. All new commands should be added here and in constructor
    if (Stricmp(command.c_str(), "CLEAR") == 0)
    {
        ClearLog();
    }
    else if (Stricmp(command.c_str(), "HELP") == 0)
    {
        AddLog("Commands:");
        for (int i = 0; i < m_Commands.Size; ++i)
        {
            AddLog("- %s", m_Commands[i]);
        }
    }
    else if (Stricmp(command.c_str(), "HISTORY") == 0)
    {
        if (arguments[0] != "-1")
        {
            int index;
            for (int i = 0; i < arguments.size(); ++i)
            {
                if (std::isdigit(arguments[i][0]))
                {
                    index = std::stoi(arguments[i]);
                    if (index != -1 && index < m_History.Size)
                    {
                        AddLog("%3d: %s\n", index, m_History[index]);
                    }
                    else if (index >= m_History.Size)
                    {
                        AddLog("Index %d is out of range for HISTORY\n", index);
                    }
                }
                else
                {
                    AddLog("%s is not a number\n", arguments[i].c_str());
                }
            }
        }
        else
        {
            int first = m_History.Size - 10;
            for (int i = first > 0 ? first : 0; i < m_History.Size; ++i)
            {
                AddLog("%3d: %s\n", i, m_History[i]);
            }
        }
    }
    else if (Stricmp(command.c_str(), "BOUNDINGBOX") == 0)
    {
        if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
        {
            if (Stricmp(arguments[0].c_str(), "TRUE") == 0 || Stricmp(arguments[0].c_str(), "ON") == 0)
            {
                m_BoolMap["boundingBoxToggle"] = true;
                AddLog("Bounding boxes have been turned on\n");
            }
            else if (Stricmp(arguments[0].c_str(), "FALSE") == 0 || Stricmp(arguments[0].c_str(), "OFF") == 0)
            {
                m_BoolMap["boundingBoxToggle"] = false;
                AddLog("Bounding boxes have been turned off\n");
            }
            else if (Stricmp(arguments[0].c_str(), "-1") == 0)
            {
                m_BoolMap["boundingBoxToggle"] = !m_BoolMap["boundingBoxToggle"];
                AddLog("Bounding boxes have been turned %s\n", m_BoolMap["boundingBoxToggle"] ? "on" : "off");
            }
            else
            {
                AddLog("Valid arguments are:\n\tTRUE\n\tFALSE\n\tON\n\tOFF");
            }
        }
        else
        {
            AddLog("Developer mode bounding boxes have not been activated\n");
        }
    }
    else if (Stricmp(command.c_str(), "RESET") == 0)
    {
        m_BoolMap["reset"] = true;
    }
    else if (Stricmp(command.c_str(), "CONNECT") == 0)
    {
        if (arguments.size() == 1 && std::atoi(Option::GetInstance().GetVariable("i_network").c_str()) == 1)
        {
            EventBus::GetInstance().Publish(&ConnectToServer(arguments.at(0).c_str()));
        }
    }
    else if (Stricmp(command.c_str(), "DISCONNECT") == 0)
    {
        EventBus::GetInstance().Publish(&Disconnect());
    }
    else if (Stricmp(command.c_str(), "HARVEST") == 0)
    {
        int currency = std::stoi(arguments.at(0));
        if (currency != -1)
        {
            Player::GetInstance().GetPlayer()->GetComponent<component::CurrencyComponent>()->ChangeBalance(currency);
        }
        else
        {
            Player::GetInstance().GetPlayer()->GetComponent<component::CurrencyComponent>()->ChangeBalance(500);
        }
    }
    else if (Stricmp(command.c_str(), "GODMODE") == 0)
    {
        Player::GetInstance().GetPlayer()->GetComponent<component::MeleeComponent>()->SetDamage(10000);
        Player::GetInstance().GetPlayer()->GetComponent<component::MeleeComponent>()->SetAttackInterval(0.01);
        Player::GetInstance().GetPlayer()->GetComponent<component::RangeComponent>()->SetDamage(10000);
        Player::GetInstance().GetPlayer()->GetComponent<component::RangeComponent>()->SetAttackInterval(0.01);
        Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->SetMaxHealth(1000000);
        Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->SetHealth(1000000);
    }
    else if (Stricmp(command.c_str(), "KILLPLAYER") == 0)
    {
        Player::GetInstance().GetPlayer()->GetComponent<component::HealthComponent>()->SetHealth(0);
    }
    else if (Stricmp(command.c_str(), "SKIPLEVEL") == 0)
    {  
        EventBus::GetInstance().Publish(&LevelDone());
    }
    else
    {
        AddLog("Unknown command: '%s'\n", command.c_str());
    }

    // On command input, we scroll to bottom
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
    m_NumberOfDebuggingLines = 4;
    if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
    {
        ++m_NumberOfDebuggingLines;
    }

    ClearLog();
    memset(m_InputBuf, 0, sizeof(m_InputBuf));
    m_HistoryPos = -1;

    // Add the valid commands to a vector. All new commands should be added here and in ExecCommand
    m_Commands.push_back("HELP");
    m_Commands.push_back("HISTORY");
    m_Commands.push_back("CLEAR");
    m_Commands.push_back("RESET");
    m_Commands.push_back("HARVEST");
    m_Commands.push_back("GODMODE");
    m_Commands.push_back("KILLPLAYER");
    m_Commands.push_back("SKIPLEVEL");
    if (std::atoi(Option::GetInstance().GetVariable("i_network").c_str()) == 1)
    {
        m_Commands.push_back("CONNECT");
        m_Commands.push_back("DISCONNECT");
    }
    m_ScrollToBottom = false;

    if (DEVELOPERMODE_DRAWBOUNDINGBOX == true)
    {
        m_Commands.push_back("BOUNDINGBOX");
        m_BoolMap["boundingBoxToggle"] = false;
    }

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
