#include "stdafx.h"
#include <vector>
#include "TextComponent.h"

#include "../Renderer/Text.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/SwapChain.h"
#include "../Renderer/DescriptorHeap.h"

#include "../Misc/Window.h"

namespace component
{
	TextComponent::TextComponent(Entity* parent, Font* font)
		:Component(parent)
	{
		m_pFont = font;
	}

	TextComponent::~TextComponent()
	{
		for (auto textMap : m_TextMap)
		{
			delete textMap.second;
		}

		m_TextMap.clear();
	}

	std::map<std::string, TextData>* const TextComponent::GetTextDataMap()
	{
		return &m_TextDataMap;
	}

	TextData* TextComponent::GetTextData(std::string name)
	{
		return &m_TextDataMap[name];
	}

	void TextComponent::AddText(std::string name)
	{
		// Default text
		TextData textData = {};
		textData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		textData.padding = { 0.5f, 0.0f };
		textData.pos = { 0.5f, 0.5f };
		textData.scale = { 1.0f, 1.0f };
		textData.text = L"DEFAULT_TEXT";

		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				Log::PrintSeverity(Log::Severity::WARNING, "It already exists a text with the name '%s'! Overwriting text data...\n", name.c_str());
			}
		}

		m_TextDataMap[name] = textData;
	}

	void TextComponent::SubmitText(Text* text, std::string name)
	{
		m_TextMap.insert({ name, text });
	}

	void TextComponent::ReplaceText(Text* text, std::string name)
	{
		bool found = false;
		for (auto textMap : m_TextMap)
		{
			if (textMap.first == name)
			{
				delete m_TextMap[name];
				m_TextMap[name] = text;
				found = true;
			}
		}

		if (found == false)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Could not find any text called '%s' to replace!\n", name);
		}
	}

	void TextComponent::UploadText(std::string name)
	{
		int numOfCharacters = GetNumOfCharacters(name);
		auto textData = GetTextData(name);

		Renderer* renderer = &Renderer::GetInstance();

		Text* text = new Text(
			renderer->m_pDevice5,
			renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
			numOfCharacters,
			m_pFont->texture);
		text->SetTextData(textData, m_pFont);

		// Look if the text exists
		bool exists = false;
		for (auto textMap : m_TextMap)
		{
			if (textMap.first == name)
			{
				exists = true;
			}
		}

		if (exists == true)
		{
			ReplaceText(text, name);
		}
		else
		{
			SubmitText(text, name);
		}

		renderer->submitTextToGPU(text, this);

		renderer->executeCopyOnDemand();
	}

	void TextComponent::SetFont(Font* font)
	{
		m_pFont = font;
	}

	void TextComponent::SetText(std::string text, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].text = to_wstring(text);
				exists = true;
			}
		}
		
		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set text.\n", name.c_str());
		}
	}

	void TextComponent::SetPos(float2 textPos, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].pos = textPos;
				exists = true;
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set position.\n", name.c_str());
		}
	}

	void TextComponent::SetScale(float2 scale, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				// Scale with the size of the window
				Renderer* renderer = &Renderer::GetInstance();			
				HWND* hwnd = const_cast<HWND*>(renderer->m_pWindow->GetHwnd());
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
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set scale.\n", name.c_str());
		}
	}

	void TextComponent::SetPadding(float2 padding, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].padding = padding;
				exists = true;
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set padding.\n", name.c_str());
		}
	}

	void TextComponent::SetColor(float4 color, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].color = color;
				exists = true;
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set color.\n", name.c_str());
		}
	}

	Font* TextComponent::GetFont() const
	{
		return m_pFont;
	}

	Texture* TextComponent::GetTexture() const
	{
		return m_pFont->texture;
	}

	Text* TextComponent::GetText(std::string name)
	{
 		return m_TextMap[name];
	}

	std::map<std::string, Text*>* TextComponent::GetTextMap()
	{
		return &m_TextMap;
	}

	const int TextComponent::GetNumOfTexts() const
	{
		return m_TextMap.size();
	}

	const int TextComponent::GetNumOfCharacters(std::string name)
	{
		return m_TextDataMap[name].text.size();
	}

	void TextComponent::Update(double dt)
	{
	}

	void TextComponent::InitScene()
	{
		Renderer::GetInstance().InitTextComponent(GetParent());
	}
}
