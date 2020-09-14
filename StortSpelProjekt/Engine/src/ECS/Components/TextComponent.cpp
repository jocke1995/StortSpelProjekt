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

	std::vector<TextData>* const TextComponent::GetTextDataVec()
	{
		return &m_TextDataVec;
	}

	void TextComponent::AddText()
	{
		// Default text
		TextData textData = {};
		textData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		textData.padding = { 0.5f, 0.0f };
		textData.pos = { 0.5f, 0.5f };
		textData.scale = { 1.0f, 1.0f };
		textData.text = L"DEFAULT_TEXT";

		m_TextDataVec.push_back(textData);
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

	void TextComponent::SetText(std::string text, int pos)
	{
		m_TextDataVec.at(pos).text = to_wstring(text);
	}

	void TextComponent::SetPos(float2 textPos, int pos)
	{
		m_TextDataVec.at(pos).pos = textPos;
	}

	void TextComponent::SetScale(float2 scale, int pos)
	{
		m_TextDataVec.at(pos).scale = scale;
	}

	void TextComponent::SetPadding(float2 padding, int pos)
	{
		m_TextDataVec.at(pos).padding = padding;
	}

	void TextComponent::SetColor(float4 color, int pos)
	{
		m_TextDataVec.at(pos).color = color;
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

	const int TextComponent::GetNumOfCharacters(int pos) const
	{
		return m_TextDataVec.at(pos).text.size();
	}

	void TextComponent::Update(double dt)
	{
	}
}
