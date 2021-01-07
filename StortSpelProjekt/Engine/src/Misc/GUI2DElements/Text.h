#ifndef TEXT_H
#define TEXT_H

class Window;
class Resource;
class ShaderResourceView;
class ConstantBuffer;
class DescriptorHeap;
class Texture;
class Renderer;
class Font;
struct SlotInfo;

// DX12 Forward Declarations
struct ID3D12Device5;

struct TextVertex
{
	DirectX::XMFLOAT4 texCoord;
};

struct TextData
{
	std::wstring text = L"";
	float2 padding = { 0.0f, 0.0f };
};

class Text
{
public:
	Text(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV, Texture* texture, TextData* textData, Font* font);
	~Text();

	SlotInfo* const GetSlotInfo() const;
	TextData* const GetTextData();
	Font* const GetFont() const;
	const int GetNrOfCharacters() const;
	const float4 GetAmountOfBlend() const;
	float4 GetColor() const;
	float2 GetScale() const;
	float2 GetPos() const;

	void SetTextDataMap(TextData* data);

private:
	friend class Renderer;
	friend class SceneManager;
	friend class CopyOnDemandTask;
	friend class TextTask;
	friend class TextManager;

	Font* m_pFont = nullptr;

	int m_NrOfCharacters = 0;
	int m_SizeOfVertices = 0;

	float2 m_Pos;
	float2 m_Scale;
	float4 m_Color;
	float4 m_BlendFactor;

	// this will store our font information
	TextData m_TextData = {};
	std::vector<TextVertex> m_TextVertexVec = {};
	std::vector<CB_PER_GUI_STRUCT> m_CBVec = {};

	Resource* m_pUploadResourceVertices = nullptr;
	Resource* m_pDefaultResourceVertices = nullptr;

	ShaderResourceView* m_pSRV = nullptr;
	ConstantBuffer* m_pCB = nullptr;

	SlotInfo* m_pSlotInfo = nullptr;

	void updateVertexData();
	void initMeshData(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV, Texture* texture);
	void createCB(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV);
	void updateCBbuffer(std::string prop = "", float4 value = { 0.0f, 0.0f, 0.0f, 0.0f });
};

#endif