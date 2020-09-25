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

private:
	ImGuiHandler();
	~ImGuiHandler();

	std::map<std::string, ImVec4> m_Vec4Map;
	std::map<std::string, float> m_FloatMap;
};

#endif