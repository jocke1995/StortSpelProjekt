#include "stdafx.h"
#include "Text.h"

#include "../Renderer/Texture/Texture.h"
#include "../Misc/Window.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/Renderer.h"
#include "Font.h"

Text::Text(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV, int numOfCharacters, Texture* texture)
{
	// Four vertices (quad) per character 
	m_NrOfVertices = numOfCharacters;
	m_SizeOfVertices = m_NrOfVertices * sizeof(TextVertex);

	// Set vertices
	m_pUploadResourceVertices = new Resource(device, m_SizeOfVertices, RESOURCE_TYPE::UPLOAD, L"Vertex_TEXT_UPLOAD_RESOURCE");
	m_pDefaultResourceVertices = new Resource(device, m_SizeOfVertices, RESOURCE_TYPE::DEFAULT, L"Vertex_TEXT_DEFAULT_RESOURCE");

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
	dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	dsrv.Buffer.FirstElement = 0;
	dsrv.Format = DXGI_FORMAT_UNKNOWN;
	dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dsrv.Buffer.NumElements = m_NrOfVertices;
	dsrv.Buffer.StructureByteStride = sizeof(TextVertex);

	m_pSRV = new ShaderResourceView(
		device,
		descriptorHeap_SRV,
		&dsrv,
		m_pDefaultResourceVertices);

	m_pSlotInfo = new SlotInfo();
	m_pSlotInfo->vertexDataIndex = m_pSRV->GetDescriptorHeapIndex();
	m_pSlotInfo->textureAlbedo = texture->GetDescriptorHeapIndex();
}

Text::~Text()
{
	delete m_pSlotInfo;

	delete m_pUploadResourceVertices;
	delete m_pDefaultResourceVertices;

	delete m_pSRV;
}

void Text::initVertexData()
{
	float2 pos = m_TextData.pos;
	float2 padding = m_TextData.padding;
	float2 scale = m_TextData.scale;
	float4 color = m_TextData.color;
	std::wstring text = m_TextData.text;
	Font font = *m_pFont;

	int numCharacters = 0;

	float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

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
			y -= (font.m_LineHeight + verticalPadding) * scale.y;
			continue;
		}

		// don't overflow the buffer
		if (numCharacters >= s_MaxNumTextCharacters)
		{
			break;
		}

		float kerning = 0.0f;
		if (i > 0)
		{
			kerning = font.GetKerning(lastChar, c);
		}

		TextVertex vTmp = {};
		vTmp.color = DirectX::XMFLOAT4{ color.x, color.y, color.z, color.w };
		vTmp.texCoord = DirectX::XMFLOAT4{ fc->u, fc->v, fc->twidth, fc->theight };
		vTmp.pos = DirectX::XMFLOAT4
		{
			x + ((fc->xoffset + kerning) * scale.x),
			y - (fc->yoffset * scale.y),
			fc->width * scale.x,
			fc->height * scale.y
		};

		m_TextVertexVec.push_back(vTmp);

		numCharacters++;

		// remove horrizontal padding and advance to next char position
		x += (fc->xadvance - horrizontalPadding) * scale.x;

		lastChar = c;
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
	return m_TextData.text.size();
}

const float4 Text::GetAmountOfBlend() const
{
	return m_TextData.blendFactor;
}

void Text::SetTextData(TextData* textData, Font* font)
{
	m_TextData = *textData;
	m_pFont = font;

	initVertexData();
}