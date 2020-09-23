#include "stdafx.h"
#include <vector>
#include "TextComponent.h"
#include "../Renderer/Text.h"
#include "../Renderer/Texture.h"
#include "../Misc/Window.h"

namespace component
{
	TextComponent::TextComponent(Entity* parent, std::pair<Font*, Texture*> font)
		:Component(parent)
	{
		m_pFont = font.first;
		m_pFontTexture = font.second;
	}

	TextComponent::~TextComponent()
	{
		for (int i = 0; i < m_TextVec.size(); i++)
		{
			delete m_TextVec.at(i);
		}

		m_TextVec.clear();
	}

	std::map<std::string, TextData>* const TextComponent::GetTextDataMap()
	{
		return &m_TextDataMap;
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

		m_TextDataMap[name] = (textData);
	}

	void TextComponent::SubmitText(Text* text)
	{
		m_TextVec.push_back(text);
	}

	void TextComponent::SetFont(std::pair<Font*, Texture*> font)
	{
		m_pFont = font.first;
		m_pFontTexture = font.second;
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
				m_TextDataMap[name].scale = scale;
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
		return m_pFontTexture;
	}

	Text* TextComponent::GetText(int pos) const
	{
 		return m_TextVec.at(pos);
	}

	const int TextComponent::GetNumOfTexts() const
	{
		return m_TextVec.size();
	}

	const int TextComponent::GetNumOfCharacters(std::string name)
	{
		return m_TextDataMap[name].text.size();
	}

	void TextComponent::Update(double dt)
	{
	}
	void TextComponent::Init(Renderer* renderer)
	{
		Log::Print("TextComponent Init called!\n");
	}
}
