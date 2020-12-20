#include "stdafx.h"
#include "Text.h"

#include "../Renderer/Texture/Texture.h"
#include "../Misc/Window.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/GPUMemory/ConstantBuffer.h"
#include "Font.h"
#include "../Headers/structs.h"

Text::Text(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV, Texture* texture, TextData* textData, Font* font)
{
	m_TextData = *textData;
	m_pFont = font;

	m_Pos = { 0.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f };
	m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_BlendFactor = { 1.0f, 1.0f, 1.0f, 1.0f };

	m_CBVec.resize(MAX_NUM_CHARACTERS);

	for (int i = 0; i < MAX_NUM_CHARACTERS; i++)
	{
		m_CBVec.at(i).blendFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_CBVec.at(i).color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_CBVec.at(i).pos = { 0.0f, 0.0f };
		m_CBVec.at(i).scale = { 1.0f, 1.0f };
		m_CBVec.at(i).textureInfo = { 0.0f, 0.0f };
	}

	updateVertexData();
}

Text::~Text()
{
	delete m_pSlotInfo;

	delete m_pUploadResourceVertices;
	delete m_pDefaultResourceVertices;

	delete m_pSRV;

	delete m_pCB;
}

void Text::updateVertexData()
{
	float2 padding = m_TextData.padding;
	std::wstring text = m_TextData.text;
	Font font = *m_pFont;

	int numCharacters = 0;

	float horrizontalPadding = (font.m_Leftpadding + font.m_Rightpadding) * padding.x;
	float verticalPadding = (font.m_Toppadding + font.m_Bottompadding) * padding.y;

	wchar_t lastChar = -1; // no last character to start with

	m_TextVertexVec.clear();

	for (int i = 0; i < text.size(); ++i)
	{
		wchar_t c = text[i];

		FontChar* fc = font.GetChar(c);

		// character not in font char set
		if (fc == nullptr)
		{
			continue;
		}

		// end of string
		if (c == L'\0')
		{
			break;
		}

		// new line
		if (c == L'\n')
		{
			continue;
		}

		// space
		if (c == L' ')
		{
			continue;
		}

		// don't overflow the buffer
		if (numCharacters >= MAX_NUM_CHARACTERS)
		{
			break;
		}

		float kerning = 0.0f;
		if (i > 0)
		{
			kerning = font.GetKerning(lastChar, c);
		}

		TextVertex vTmp = {};
		vTmp.texCoord = DirectX::XMFLOAT4{ fc->u, fc->v, fc->twidth, fc->theight };
		m_TextVertexVec.push_back(vTmp);

		numCharacters++;

		lastChar = c;
	}

	m_NrOfCharacters = numCharacters;
}

void Text::initMeshData(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV, Texture* texture)
{
	// Set vertices
	m_SizeOfVertices = m_NrOfCharacters * sizeof(TextVertex);

	if (m_pUploadResourceVertices != nullptr)
	{
		delete m_pUploadResourceVertices;
		m_pUploadResourceVertices = nullptr;
	}
	if (m_pDefaultResourceVertices != nullptr)
	{
		delete m_pDefaultResourceVertices;
		m_pDefaultResourceVertices = nullptr;
	}

	m_pUploadResourceVertices = new Resource(device, m_SizeOfVertices, RESOURCE_TYPE::UPLOAD, L"VERTEX_TEXT_UPLOAD_RESOURCE");
	m_pDefaultResourceVertices = new Resource(device, m_SizeOfVertices, RESOURCE_TYPE::DEFAULT, L"VERTEX_TEXT_DEFAULT_RESOURCE");

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
	dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	dsrv.Buffer.FirstElement = 0;
	dsrv.Format = DXGI_FORMAT_UNKNOWN;
	dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dsrv.Buffer.NumElements = m_NrOfCharacters;
	dsrv.Buffer.StructureByteStride = sizeof(TextVertex);

	if (m_pSRV != nullptr)
	{
		delete m_pSRV;
		m_pSRV = nullptr;
	}

	m_pSRV = new ShaderResourceView(
		device,
		descriptorHeap_SRV,
		&dsrv,
		m_pDefaultResourceVertices);

	if (m_pSlotInfo != nullptr)
	{
		delete m_pSlotInfo;
		m_pSlotInfo = nullptr;
	}

	m_pSlotInfo = new SlotInfo();

	m_pSlotInfo->vertexDataIndex = m_pSRV->GetDescriptorHeapIndex();
	m_pSlotInfo->textureAlbedo = texture->GetDescriptorHeapIndex();
}

void Text::createCB(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV)
{
	// Create ConstantBuffer
	int sizeOfTextData = sizeof(CB_PER_TEXT_STRUCT);
	
	if (m_pCB != nullptr)
	{
		delete m_pCB;
		m_pCB = nullptr;
	}

	m_pCB = new ConstantBuffer(device, sizeOfTextData, L"CB_TEXTDATA_RESOURCE", descriptorHeap_CBV);
}

void Text::updateCBbuffer(std::string prop, float4 value)
{
	float2 padding = m_TextData.padding;
	std::wstring text = m_TextData.text;
	Font font = *m_pFont;

	if (prop == "pos")
	{
		m_Pos.x = value.x;
		m_Pos.y = value.y;
	}
	else if (prop == "scale")
	{
		m_Scale.x = value.x;
		m_Scale.y = value.y;
	}
	else if (prop == "color")
	{
		m_Color = value;
	}
	else if (prop == "blendFactor")
	{
		m_BlendFactor = value;
	}

	int numCharacters = 0;

	float topLeftScreenX = (m_Pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - m_Pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;

	float horrizontalPadding = (font.m_Leftpadding + font.m_Rightpadding) * padding.x;
	float verticalPadding = (font.m_Toppadding + font.m_Bottompadding) * padding.y;

	wchar_t lastChar = -1; // no last character to start with

	for (int i = 0; i < text.size(); ++i)
	{
		wchar_t c = text[i];

		FontChar* fc = font.GetChar(c);

		// character not in font char set
		if (fc == nullptr)
		{
			continue;
		}

		// end of string
		if (c == L'\0')
		{
			break;
		}

		// new line
		if (c == L'\n')
		{
			x = topLeftScreenX;
			y -= (font.m_LineHeight + verticalPadding) * m_Scale.y;
			continue;
		}

		// space
		if (c == L' ')
		{
			x += (fc->xadvance - horrizontalPadding) * m_Scale.x;
			continue;
		}

		// don't overflow the buffer
		if (numCharacters >= MAX_NUM_CHARACTERS)
		{
			break;
		}

		float kerning = 0.0f;
		if (i > 0)
		{
			kerning = font.GetKerning(lastChar, c);
		}

		m_CBVec.at(numCharacters).blendFactor = m_BlendFactor;
		m_CBVec.at(numCharacters).color = m_Color;
		m_CBVec.at(numCharacters).pos = float4
		{
			x + ((fc->xoffset + kerning) * m_Scale.x),
			y - (fc->yoffset * m_Scale.y),
			fc->width * m_Scale.x,
			fc->height * m_Scale.y
		};

		// remove horrizontal padding and advance to next char position
		x += (fc->xadvance - horrizontalPadding) * m_Scale.x;

		lastChar = c;

		numCharacters++;
	}
}

SlotInfo* const Text::GetSlotInfo() const
{
	return m_pSlotInfo;
}

TextData* const Text::GetTextData()
{
	return &m_TextData;
}

Font* const Text::GetFont() const
{
	return m_pFont;
}

int const Text::GetNrOfCharacters() const
{
	return m_NrOfCharacters;
}

const float4 Text::GetAmountOfBlend() const
{
	return m_BlendFactor;
}

float4 Text::GetColor() const
{
	return m_Color;
}

float2 Text::GetScale() const
{
	return m_Scale;
}

float2 Text::GetPos() const
{
	return m_Pos;
}

void Text::SetTextDataMap(TextData* data)
{
	m_TextData = *data;
}
