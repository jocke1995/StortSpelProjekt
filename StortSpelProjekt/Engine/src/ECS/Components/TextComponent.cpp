#include "stdafx.h"
#include <vector>
#include "TextComponent.h"
#include "../Renderer/Text.h"
#include "../Misc/Window.h"

namespace component
{
	TextComponent::TextComponent(Entity* parent)
		:Component(parent)
	{
	}

	TextComponent::~TextComponent()
	{
		for (int i = 0; i < m_TextDataVec.size(); i++)
		{
			delete m_TextDataVec.at(i)->font.kerningsList;
			delete m_TextDataVec.at(i)->font.charList;
			delete m_TextDataVec.at(i);
		}
		m_TextDataVec.clear();
	}

	/*void TextComponent::SetText(Text* text, int pos)
	{
		m_TextDataVec.at(pos)->text = text->GetText();
		m_TextDataVec.at(pos)->font = text->GetFont();
		m_TextDataVec.at(pos)->padding = text->GetPadding();
		m_TextDataVec.at(pos)->pos = text->GetPos();
		m_TextDataVec.at(pos)->scale = text->GetScale();
		m_TextDataVec.at(pos)->color = text->GetColor();
	}*/

	std::vector<TextData*> const TextComponent::GetTextDataVec() const
	{
		return m_TextDataVec;
	}

	void TextComponent::AddText()
	{
		TextData* textData = new TextData();
		m_TextDataVec.push_back(textData);
	}

	void TextComponent::SetFont(Window* window, std::string fontPath, int pos)
	{
		m_TextDataVec.at(pos)->font = LoadFont(std::wstring(fontPath.begin(), fontPath.end()).c_str(), window->GetScreenWidth(), window->GetScreenHeight());
	}

	void TextComponent::SetText(std::string text, int pos)
	{
		m_TextDataVec.at(pos)->text = std::wstring(text.begin(), text.end()).length();
	}

	void TextComponent::SetPos(float2 textPos, int pos)
	{
		m_TextDataVec.at(pos)->pos = textPos;
	}

	void TextComponent::SetScale(float2 scale, int pos)
	{
		m_TextDataVec.at(pos)->scale = scale;
	}

	void TextComponent::SetPadding(float2 padding, int pos)
	{
		m_TextDataVec.at(pos)->padding = padding;
	}

	void TextComponent::SetColor(float4 color, int pos)
	{
		m_TextDataVec.at(pos)->color = color;
	}

	void TextComponent::Update(double dt)
	{
	}
}
