#include "stdafx.h"
#include "Font.h"

#include "../Renderer/Texture/Texture.h"

Font::Font()
{
}

Font::~Font()
{
}

float Font::GetKerning(wchar_t first, wchar_t second) const
{
	for (int i = 0; i < m_NumKernings; ++i)
	{
		if ((wchar_t)m_pKerningsList[i].firstid == first && (wchar_t)m_pKerningsList[i].secondid == second)
		{
			return m_pKerningsList[i].amount;
		}
	}
	return 0.0f;
}

FontChar* Font::GetChar(wchar_t c) const
{
	for (int i = 0; i < m_NumCharacters; ++i)
	{
		if (c == (wchar_t)m_pCharList[i].id)
		{
			return &m_pCharList[i];
		}
	}
	return nullptr;
}

Texture* const Font::GetTexture() const
{
	return m_pTexture;
}

std::wstring Font::GetName() const
{
	return m_Name;
}
