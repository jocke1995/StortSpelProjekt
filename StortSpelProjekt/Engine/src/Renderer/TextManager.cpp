#include "stdafx.h"
#include "TextManager.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/DescriptorHeap.h"

#include "../Misc/Window.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/GUI2DElements/Text.h"
#include "../Misc/GUI2DElements/Font.h"

#include "../Renderer/DX12Tasks/DX12Task.h"
#include "../Renderer/DX12Tasks/CopyOnDemandTask.h"

TextManager::TextManager()
{
	// Default text
	m_DefaultTextData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_DefaultTextData.padding = { 0.5f, 0.0f };
	m_DefaultTextData.pos = { 0.5f, 0.5f };
	m_DefaultTextData.scale = { 1.0f, 1.0f };
	m_DefaultTextData.text = L"DEFAULT_TEXT";
}

TextManager::~TextManager()
{
	for (auto textMap : m_TextMap)
	{
		if (textMap.second != nullptr)
		{
			delete textMap.second;
		}
	}
	m_TextMap.clear();
}

std::map<std::string, TextData>* const TextManager::GetTextDataMap()
{
	return &m_TextDataMap;
}

TextData* TextManager::GetTextData(std::string name)
{
	return &m_TextDataMap[name];
}

void TextManager::AddText(std::string name)
{
	// Look if a font is chosen, otherwise, set the default font
	if (m_pFont == nullptr)
	{
		m_pFont = AssetLoader::Get()->LoadFontFromFile(L"Arial.fnt");
	}

	// Default text
	TextData textData = m_DefaultTextData;

	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		Log::PrintSeverity(Log::Severity::WARNING, "It already exists a text with the name '%s'! Overwriting text data...\n", name.c_str());
	}

	m_TextDataMap[name] = textData;
}

void TextManager::SetFont(Font* font)
{
	m_pFont = font;
}

void TextManager::SetText(std::string text, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextDataMap[name].text = to_wstring(text);
		exists = true;
		uploadTextData(name, &Renderer::GetInstance());
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set text.\n", name.c_str());
	}
}

void TextManager::SetPos(float2 textPos, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextDataMap[name].pos = textPos;
		exists = true;
		uploadTextData(name, &Renderer::GetInstance());
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set position.\n", name.c_str());
	}
}

void TextManager::SetScale(float2 scale, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		// Scale with the size of the window
		Renderer* renderer = &Renderer::GetInstance();
		HWND* hwnd = const_cast<HWND*>(renderer->GetWindow()->GetHwnd());
		RECT rect;

		float win_x = 0, win_y = 0;
		if (GetWindowRect(*hwnd, &rect))
		{
			win_x = rect.right - rect.left;
			win_y = rect.bottom - rect.top;
		}

		float scale_x = 0, scale_y = 0;
		scale_x = win_x / 1000;
		scale_y = win_y / 1000;
		float aspect = scale_x / scale_y;

		m_TextDataMap[name].scale.x = (scale.x * scale_x);
		m_TextDataMap[name].scale.y = (scale.y * scale_y * aspect);

		exists = true;
		uploadTextData(name, renderer);
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set scale.\n", name.c_str());
	}
}

void TextManager::SetPadding(float2 padding, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextDataMap[name].padding = padding;
		exists = true;
		uploadTextData(name, &Renderer::GetInstance());
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set padding.\n", name.c_str());
	}
}

void TextManager::SetColor(float4 color, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextDataMap[name].color = color;
		exists = true;
		uploadTextData(name, &Renderer::GetInstance());
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set color.\n", name.c_str());
	}
}

void TextManager::SetBlend(float4 blend, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextDataMap[name].blendFactor = blend;
		exists = true;
		uploadTextData(name, &Renderer::GetInstance());
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set blend.\n", name.c_str());
	}
}

void TextManager::HideText(bool hide)
{
	m_TextIsHidden = hide;
}

const bool TextManager::IsTextHidden() const
{
	return m_TextIsHidden;
}

Font* TextManager::GetFont() const
{
	return m_pFont;
}

Texture* TextManager::GetFontTexture() const
{
	return m_pFont->GetTexture();
}

Text* TextManager::GetText(std::string name) const
{
	return m_TextMap.at(name);
}

std::map<std::string, Text*>* TextManager::GetTextMap()
{
	return &m_TextMap;
}

const int TextManager::GetNumOfTexts() const
{
	return m_TextMap.size();
}

const int TextManager::GetNumOfCharacters(std::string name) const
{
	return m_TextDataMap.at(name).text.size();
}

void TextManager::submitText(Text* text, std::string name)
{
	m_TextMap.insert({ name, text });
}

void TextManager::replaceText(Text* text, std::string name)
{
	bool found = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		// Temp code, removes the text from CopyOnDemandTask before we delete the text*
		if (m_TextMap[name] != nullptr)
		{
			CopyTask* task = Renderer::GetInstance().m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND];
			CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(task);
			codt->UnSubmitText(m_TextMap[name]);
		}

		// Temp code, needs rewrite
		Renderer::GetInstance().waitForGPU();
		delete m_TextMap[name];
		m_TextMap[name] = text;
		found = true;
	}

	if (found == false)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Could not find any text called '%s' to replace!\n", name);
	}
}

void TextManager::uploadTextData(std::string name, Renderer* renderer)
{
	int numOfCharacters = GetNumOfCharacters(name);
	auto textData = GetTextData(name);

	Text* text = new Text(
		renderer->m_pDevice5,
		renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
		numOfCharacters,
		m_pFont->GetTexture());
	text->SetTextData(textData, m_pFont);

	// Look if the text exists
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		exists = true;
	}

	if (exists == true)
	{
		// Replacing an existing text in the map
		replaceText(text, name);
	}
	else
	{
		// Adding a new text to the map
		submitText(text, name);
	}

	// Uploading the text data to the gpu
	renderer->submitTextToGPU(text, this);
}
