#ifndef TEXT_H
#define TEXT_H

#include "Renderer.h"

class Window;
class Resource;
class ShaderResourceView;
class DescriptorHeap;
class Texture;
class Renderer;
struct SlotInfo;

// DX12 Forward Declarations
struct ID3D12CommandQueue;
struct ID3D12CommandList;
struct ID3D12Fence1;
struct ID3D12Device5;

// the maximum number of characters you can render during a frame. This is just used to make sure
// there is enough memory allocated for the text vertex buffer each frame
const static int s_MaxNumTextCharacters = 1024;

struct TextVertex
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 texCoord;
	DirectX::XMFLOAT4 color;
};

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

struct Font
{
	std::wstring name; // name of the font
	std::wstring fontImage;
	int size; // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
	float lineHeight; // how far to move down to next line, will be normalized
	float baseHeight; // height of all characters, will be normalized
	int textureWidth; // width of the font texture
	int textureHeight; // height of the font texture
	int numCharacters; // number of characters in the font
	FontChar* charList = nullptr; // list of characters
	int numKernings; // the number of kernings
	FontKerning* kerningsList = nullptr; // list to hold kerning values
	Texture* texture = nullptr;

	// these are how much the character is padded in the texture. We
	// add padding to give sampling a little space so it does not accidentally
	// padd the surrounding characters. We will need to subtract these paddings
	// from the actual spacing between characters to remove the gaps you would otherwise see
	float leftpadding;
	float toppadding;
	float rightpadding;
	float bottompadding;

	// this will return the amount of kerning we need to use for two characters
	float GetKerning(wchar_t first, wchar_t second)
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

	// this will return a FontChar given a wide character
	FontChar* GetChar(wchar_t c)
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
};

struct TextData
{
	std::wstring text = L"";
	float2 pos = { 0.0f, 0.0f };
	float2 scale = { 0.0f, 0.0f };
	float2 padding = { 0.0f, 0.0f };
	float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class Text
{
public:
	Text(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV, int numOfCharacters, Texture* texture);
	~Text();

	SlotInfo* const GetSlotInfo() const;
	TextData* const GetTextData(int pos);
	const int GetNrOfCharacters() const;

	void SetTextData(TextData* textData, Font* font);

private:
	friend class Renderer;
	friend class SceneManager;

	Font* m_pFont = nullptr;

	int m_NrOfVertices = 0;
	int m_SizeOfVertices = 0;

	// this will store our font information
	TextData m_TextData = {};
	std::vector<TextVertex> m_TextVertexVec = {};

	Resource* m_pUploadResourceVertices = nullptr;
	Resource* m_pDefaultResourceVertices = nullptr;

	ShaderResourceView* m_pSRV = nullptr;

	SlotInfo* m_pSlotInfo = nullptr;

	void initVertexData();
};

#endif