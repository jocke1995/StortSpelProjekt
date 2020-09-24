#ifndef IMGUIHANDLER_H
#define IMGUIHANDLER_H

#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_win32.h"
#include "../ImGUI/imgui_impl_dx12.h"

class ImGuiHandler
{
public:
	static ImGuiHandler& GetInstance();

	void NewFrame();
	void UpdateFrame();

	void SetFloat(std::string name, float value);
	void SetVec4(std::string name, ImVec4 value);
	void SetFloat4(std::string name, float4 value);
	void SetXMFLOAT4(std::string name, DirectX::XMFLOAT4 value);

	float GetFloat(std::string name);
	ImVec4 GetVec4(std::string name);
	float4 GetFloat4(std::string name);
	DirectX::XMFLOAT4 GetXMFLOAT4(std::string name);

	// For console

	// Portable helpers
	static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
	static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
	static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

	void ClearLog();
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);
	void DrawConsole(const char* title);
	void ExecCommand(const char* command_line);
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	int TextEditCallback(ImGuiInputTextCallbackData* data);

private:
	ImGuiHandler();
	~ImGuiHandler();

	std::map<std::string, ImVec4> m_Vec4Map;
	std::map<std::string, float> m_FloatMap;
	std::map<std::string, char*> m_TextMap;

	char m_InputBuf[256];
	ImVector<char*> m_Items;
	ImVector<const char*> m_Commands;
	ImVector<char*> m_History;
	int m_HistoryPos;
	ImGuiTextFilter m_Filter;
	bool m_ScrollToBottom;
};

#endif