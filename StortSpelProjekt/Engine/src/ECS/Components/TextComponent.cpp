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

	void TextComponent::SetFont(std::pair<Font*, Texture*> font)
	{
		m_pFont = font.first;
		m_pFontTexture = font.second;
	}

	void TextComponent::SetText(std::string text, int pos)
	{
		std::wstring wtext = std::wstring(text.begin(), text.end());
		m_TextDataVec.at(pos).text = wtext;
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

	const int TextComponent::GetNumOfCharacters(int pos) const
	{
		return m_TextDataVec.at(pos).text.size();
	}

	void TextComponent::Update(double dt)
	{
	}
}
