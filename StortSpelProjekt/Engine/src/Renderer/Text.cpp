#include "Text.h"
#include "stdafx.h"
#include <fstream>

#include "Texture.h"
#include "../Misc/Window.h"
#include "Resource.h"
#include "ShaderResourceView.h"
#include "DescriptorHeap.h"

Text::Text(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV, int numOfCharacters, Texture* texture)
{
	// Four vertices per character
	m_NrOfVertices = numOfCharacters * 4;
	m_SizeOfVertices = m_NrOfVertices * sizeof(TextVertex);

	// Set vertices
	m_pUploadResourceVertices = new Resource(device, m_SizeOfVertices, RESOURCE_TYPE::UPLOAD, L"Vertex_UPLOAD_RESOURCE");
	m_pDefaultResourceVertices = new Resource(device, m_SizeOfVertices, RESOURCE_TYPE::DEFAULT, L"Vertex_DEFAULT_RESOURCE");

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
	m_pSlotInfo->textureDiffuse = texture->GetDescriptorHeapIndex();
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
	float topLeftScreenX = (m_TextData.pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - m_TextData.pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;
	
	float horrizontalPadding = (m_pFont->leftpadding + m_pFont->rightpadding) * m_TextData.padding.x;
	float verticalPadding = (m_pFont->toppadding + m_pFont->bottompadding) * m_TextData.padding.y;

	wchar_t lastChar = -1; // no last character to start with
	
	for (int nrOfCharacters = 0; nrOfCharacters < m_TextData.text.size(); ++nrOfCharacters)
	{
		wchar_t c = m_TextData.text[nrOfCharacters];

		FontChar* fc = m_pFont->GetChar(c);

		// character not in font char set
		if (fc == nullptr)
		{
			continue;
		}

		// end of string
		if (c == L' ')
		{
			break;
		}

		// new line
		if (c == L'n')
		{
			x = topLeftScreenX;
			y -= (m_pFont->lineHeight + verticalPadding) * m_TextData.scale.y;
			continue;
		}

		// don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
		if (nrOfCharacters >= g_MaxNumTextCharacters)
		{
			break;
		}

		float kerning = 0.0f;
		if (nrOfCharacters > 0)
		{
			kerning = m_pFont->GetKerning(lastChar, c);
		}

		TextVertex vTmp = {};
		vTmp.color = DirectX::XMFLOAT4{ m_TextData.color.x, m_TextData.color.y, m_TextData.color.z, m_TextData.color.w };
		vTmp.texCoord = DirectX::XMFLOAT4{ fc->u, fc->v, fc->twidth, fc->theight };
		vTmp.pos = DirectX::XMFLOAT4{ x + ((fc->xoffset + kerning) * m_TextData.scale.x), y - (fc->yoffset * m_TextData.scale.y),
			fc->width * m_TextData.scale.x, fc->height * m_TextData.scale.y };

		m_TextVertexVec.push_back(vTmp);

		// remove horrizontal padding and advance to next char position
		x += (fc->xadvance - horrizontalPadding) * m_TextData.scale.x;

		lastChar = c;
	}
}

SlotInfo* const Text::GetSlotInfo() const
{
	return m_pSlotInfo;
}

TextData* const Text::GetTextData(int pos)
{
	return &m_TextData;
}

int const Text::GetNrOfCharacters() const
{
	return m_TextData.text.size();
}

void Text::SetTextData(TextData* textData, Font* font)
{
	m_TextData = *textData;
	m_pFont = font;

	initVertexData();
}