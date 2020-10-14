#ifndef FONT_H
#define FONT_H

class Texture;

struct FontChar
{
	// the unicode id
	int id;

	// these need to be converted to texture coordinates 
	// (where 0.0 is 0 and 1.0 is textureWidth of the font)
	float u; // u texture coordinate
	float v; // v texture coordinate
	float twidth; // width of character on texture
	float theight; // height of character on texture

	float width; // width of character in screen coords
	float height; // height of character in screen coords

	// these need to be normalized based on size of font
	float xoffset; // offset from current cursor pos to left side of character
	float yoffset; // offset from top of line to top of character
	float xadvance; // how far to move to right for next character
};

struct FontKerning
{
	int firstid; // the first character
	int secondid; // the second character
	float amount; // the amount to add/subtract to second characters x
};

class Font
{
public:
	Font();
	~Font();

	// this will return the amount of kerning we need to use for two characters
	float GetKerning(wchar_t first, wchar_t second) const;
	// this will return a FontChar given a wide character
	FontChar* GetChar(wchar_t c) const;
	Texture* const GetTexture() const;
	std::wstring GetName() const;

private:
	friend class AssetLoader;
	friend class Text;

	std::wstring m_Name = L""; // name of the font
	std::wstring m_FontImage = L"";
	int m_Size = 0; // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
	float m_LineHeight = 0; // how far to move down to next line, will be normalized
	float m_BaseHeight = 0; // height of all characters, will be normalized
	int m_TextureWidth = 0; // width of the font texture
	int m_TextureHeight = 0; // height of the font texture
	int m_NumCharacters = 0; // number of characters in the font
	FontChar* m_pCharList = nullptr; // list of characters
	int m_NumKernings = 0; // the number of kernings
	FontKerning* m_pKerningsList = nullptr; // list to hold kerning values
	Texture* m_pTexture = nullptr;

	// these are how much the character is padded in the texture. We
	// add padding to give sampling a little space so it does not accidentally
	// padd the surrounding characters. We will need to subtract these paddings
	// from the actual spacing between characters to remove the gaps you would otherwise see
	float m_Leftpadding = 0;
	float m_Toppadding = 0;
	float m_Rightpadding = 0;
	float m_Bottompadding = 0;
};

#endif