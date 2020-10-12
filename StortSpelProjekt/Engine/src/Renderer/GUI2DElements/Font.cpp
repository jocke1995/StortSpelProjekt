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
	for (int i = 0; i < numKernings; ++i)
	{
		if ((wchar_t)kerningsList[i].firstid == first && (wchar_t)kerningsList[i].secondid == second)
		{
			return kerningsList[i].amount;
		}
	}
	return 0.0f;
}

FontChar* Font::GetChar(wchar_t c) const
{
	for (int i = 0; i < numCharacters; ++i)
	{
		if (c == (wchar_t)charList[i].id)
		{
			return &charList[i];
		}
	}
	return nullptr;
}

Texture* const Font::GetTexture() const
{
	return texture;
}

std::wstring Font::GetName() const
{
	return name;
}
