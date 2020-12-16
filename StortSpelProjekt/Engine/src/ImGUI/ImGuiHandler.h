#ifndef IMGUIHANDLER_H
#define IMGUIHANDLER_H

#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_win32.h"
#include "../ImGUI/imgui_impl_dx12.h"
#include <psapi.h>

class ImGuiHandler
{
public:
	static ImGuiHandler& GetInstance();

	/// <summary>
	/// Initiats a new ImGui frame for a Win32 application using DirectX12.
	/// </summary>
	void NewFrame();
	/// <summary>
	/// Updates the ImGui frame. This is where all ImGui elements to be displayed are added to the frame.
	/// </summary>
	void UpdateFrame();

	void SetBool(std::string name, bool value);
	bool GetBool(std::string name);

	/*---------------- For console ----------------*/

	/*--------- Portable helpers -----------*/
	
	/// <summary>
	/// Compares two char arrays, diregarding casing.
	/// </summary>
	/// <param name="s1">: The first char array</param>
	/// <param name="s2">: The second char array</param>
	/// <returns>0 if the char arrays are equal</returns>
	static int   Stricmp(const char* s1, const char* s2);
	/// <summary>
	/// Compares parts of two char arrays, diregarding casing.
	/// </summary>
	/// <param name="s1">: The first char array</param>
	/// <param name="s2">: The second char array</param>
	/// <returns>0 if the char arrays are equal</returns>
	static int   Strnicmp(const char* s1, const char* s2, int n);
	/// <summary>
	/// Duplicates a char array.
	/// </summary>
	/// <param name="s">: The char array to duplicate</param>
	/// <returns>The copy of the char array</returns>
	static char* Strdup(const char* s);
	/// <summary>
	/// Trims a char array, removing spaces from the end
	/// </summary>
	/// <param name="s">: The char array to trim</param>
	static void  Strtrim(char* s);

	/// <summary>
	/// Clears the log of the console.
	/// </summary>
	void ClearLog();
	/// <summary>
	/// Adds an entry to the console log.
	/// </summary>
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);
	/// <summary>
	/// Updates and draws the console window.
	/// </summary>
	/// <param name="title">: The title of the console window</param>
	void DrawConsole(const char* title);
	/// <summary>
	/// Executes a command entered in the console.
	/// </summary>
	/// <param name="command_line">: The command to execute</param>
	void ExecCommand(const char* command_line);
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	/// <summary>
	/// Enables command completion by pressing the tab key and
	/// using the arrow keys for navigating the history.
	/// </summary>
	/// <param name="data">: Data storing information about the text entry field of the console</param>
	/// <returns>0</returns>
	int TextEditCallback(ImGuiInputTextCallbackData* data);

private:
	ImGuiHandler();
	~ImGuiHandler();

	std::map<std::string, bool> m_BoolMap;

	// Buffer for text input in the console
	char m_InputBuf[256];
	// A vector containing the items in the console log
	ImVector<char*> m_Items;
	// A vector containing the valid commands
	ImVector<const char*> m_Commands;
	// A vector containing the history of the console
	ImVector<char*> m_History;
	// The current position in the history of the console
	int m_HistoryPos;
	// Used to filter the items shown in the console log
	ImGuiTextFilter m_Filter;
	// Determines whether to scroll the log to the bottom or not
	bool m_ScrollToBottom;

	// The number of lines to be shown in the debugging window.
	// Used to set the window to the correct height.
	int m_NumberOfDebuggingLines;
};

#endif