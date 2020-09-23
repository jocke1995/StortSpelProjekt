#include "ImGuiHandler.h"

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

    {
        static int counter = 0;

        ImGui::Begin("Hello there!");                          // Create a window called "Hello, world!" and append into it.
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        if (m_FloatMap.find("LightPositionZ") != m_FloatMap.end())
        {
            ImGui::SliderFloat("float", &m_FloatMap["LightPositionZ"], -15.0f, 15.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        }
        if (m_Vec4Map.find("LightColor") != m_Vec4Map.end())
        {
            ImGui::ColorEdit3("clear color", (float*)&m_Vec4Map["LightColor"]); // Edit 3 floats representing a color
        }

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
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

ImGuiHandler::ImGuiHandler()
{
}

ImGuiHandler::~ImGuiHandler()
{
}
