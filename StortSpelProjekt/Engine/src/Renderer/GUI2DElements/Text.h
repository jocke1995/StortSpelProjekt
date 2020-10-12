#ifndef TEXT_H
#define TEXT_H

#include "../Renderer/Renderer.h"

class Window;
class Resource;
class ShaderResourceView;
class DescriptorHeap;
class Texture;
class Renderer;
class Font;
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
	Font* const GetFont() const;
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