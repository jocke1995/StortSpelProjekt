#include "Text.h"
#include "stdafx.h"
#include <fstream>

#include "Texture.h"
#include "../Misc/Window.h"
#include "Resource.h"
#include "ShaderResourceView.h"
#include "DescriptorHeap.h"

// TODO Move font to assetloader so that we cant load more than one of the same fonts
Font LoadFont(LPCWSTR filename, int windowWidth, int windowHeight)
{
	std::wifstream fs;
	fs.open(filename);

	Font font;
	std::wstring tmp;
	int startpos;

	// extract font name
	fs >> tmp >> tmp; // info face = fontname
	startpos = tmp.find(L"\"") + 1;
	font.name = tmp.substr(startpos, tmp.size() - startpos - 1);

	// get font size
	fs >> tmp; // size=73
	startpos = tmp.find(L"=") + 1;
	font.size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
	fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

	// get padding
	fs >> tmp; // padding=5,5,5,5 
	startpos = tmp.find(L"=") + 1;
	tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

	// get up padding
	startpos = tmp.find(L",") + 1;
	font.toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get right padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	font.rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get down padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	font.bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// get left padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	font.leftpadding = std::stoi(tmp) / (float)windowWidth;

	fs >> tmp; // spacing=0,0

	// get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp >> tmp; // common lineHeight=95
	startpos = tmp.find(L"=") + 1;
	font.lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp; // base=68
	startpos = tmp.find(L"=") + 1;
	font.baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// get texture width
	fs >> tmp; // scaleW=512
	startpos = tmp.find(L"=") + 1;
	font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get texture height
	fs >> tmp; // scaleH=512
	startpos = tmp.find(L"=") + 1;
	font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// get pages, packed, page id
	fs >> tmp >> tmp; // pages=1 packed=0
	fs >> tmp >> tmp; // page id=0

	// get texture filename
	std::wstring wtmp;
	fs >> wtmp; // file="Arial.png"
	startpos = wtmp.find(L"\"") + 1;
	font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);
	font.fontImage = L"../Vendor/Resources/Fonts/" + font.fontImage;

	// get number of characters
	fs >> tmp >> tmp; // chars count=97
	startpos = tmp.find(L"=") + 1;
	font.numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the character list
	font.charList = new FontChar[font.numCharacters];

	for (int c = 0; c < font.numCharacters; ++c)
	{
		// get unicode id
		fs >> tmp >> tmp; // char id=0
		startpos = tmp.find(L"=") + 1;
		font.charList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get x
		fs >> tmp; // x=392
		startpos = tmp.find(L"=") + 1;
		font.charList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureWidth;

		// get y
		fs >> tmp; // y=340
		startpos = tmp.find(L"=") + 1;
		font.charList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureHeight;

		// get width
		fs >> tmp; // width=47
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		font.charList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
		font.charList[c].twidth = (float)std::stoi(tmp) / (float)font.textureWidth;

		// get height
		fs >> tmp; // height=57
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		font.charList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
		font.charList[c].theight = (float)std::stoi(tmp) / (float)font.textureHeight;

		// get xoffset
		fs >> tmp; // xoffset=-6
		startpos = tmp.find(L"=") + 1;
		font.charList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get yoffset
		fs >> tmp; // yoffset=16
		startpos = tmp.find(L"=") + 1;
		font.charList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

		// get xadvance
		fs >> tmp; // xadvance=65
		startpos = tmp.find(L"=") + 1;
		font.charList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// get page
		// get channel
		fs >> tmp >> tmp; // page=0    chnl=0
	}

	// get number of kernings
	fs >> tmp >> tmp; // kernings count=96
	startpos = tmp.find(L"=") + 1;
	font.numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the kernings list
	font.kerningsList = new FontKerning[font.numKernings];

	for (int k = 0; k < font.numKernings; ++k)
	{
		// get first character
		fs >> tmp >> tmp; // kerning first=87
		startpos = tmp.find(L"=") + 1;
		font.kerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get second character
		fs >> tmp; // second=45
		startpos = tmp.find(L"=") + 1;
		font.kerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get amount
		fs >> tmp; // amount=-1
		startpos = tmp.find(L"=") + 1;
		int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
		font.kerningsList[k].amount = (float)t / (float)windowWidth;
	}

	return font;
}

Text::Text(std::wstring fontPath, ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV)
{
	m_pFontTexture = new Texture();
	m_pFontTexture->Init(fontPath, device, descriptorHeap_SRV);

	m_NrOfVertices = g_MaxNumTextCharacters * 4;
	m_SizeOfVertices = m_NrOfVertices * sizeof(TextVertex);
	m_NrOfCharacters = 0;

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
}

Text::~Text()
{
	delete m_pSlotInfo;

	delete m_pUploadResourceVertices;
	delete m_pDefaultResourceVertices;

	delete m_pSRV;

	delete m_pFontTexture;
}

void Text::InitVertexData()
{
	float topLeftScreenX = (m_Pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - m_Pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;

	float horrizontalPadding = (m_pFont.leftpadding + m_pFont.rightpadding) * m_Padding.x;
	float verticalPadding = (m_pFont.toppadding + m_pFont.bottompadding) * m_Padding.y;

	wchar_t lastChar = -1; // no last character to start with

	for (int nrOfCharacters = 0; nrOfCharacters < m_Text.size(); ++nrOfCharacters)
	{
		wchar_t c = m_Text[nrOfCharacters];

		FontChar* fc = m_pFont.GetChar(c);

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
			y -= (m_pFont.lineHeight + verticalPadding) * m_Scale.y;
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
			kerning = m_pFont.GetKerning(lastChar, c);
		}

		TextVertex vTmp = {};
		vTmp.color = DirectX::XMFLOAT4{ m_Color.x, m_Color.y, m_Color.z, m_Color.w };
		vTmp.texCoord = DirectX::XMFLOAT4{ fc->u, fc->v, fc->twidth, fc->theight };
		vTmp.pos = DirectX::XMFLOAT4{ x + ((fc->xoffset + kerning) * m_Scale.x), y - (fc->yoffset * m_Scale.y),
			fc->width * m_Scale.x, fc->height * m_Scale.y };

		m_TextVertexVec.push_back(vTmp);
		/*TextVertex(m_Color.x,
		m_Color.y,
		m_Color.z,
		m_Color.w,
		fc->u,
		fc->v,
		fc->twidth,
		fc->theight,
		x + ((fc->xoffset + kerning) * m_Scale.x),
		y - (fc->yoffset * m_Scale.y),
		fc->width * m_Scale.x,
		fc->height * m_Scale.y);*/

		// remove horrizontal padding and advance to next char position
		x += (fc->xadvance - horrizontalPadding) * m_Scale.x;

		lastChar = c;
	}
}

Font Text::GetFont() const
{
	return m_pFont;
}

std::wstring const Text::GetText() const
{
	return m_Text;
}

float2 const Text::GetPos() const
{
	return m_Pos;
}

float2 const Text::GetScale() const
{
	return m_Scale;
}

float2 const Text::GetPadding() const
{
	return m_Padding;
}

float4 const Text::GetColor() const
{
	return m_Color;
}

SlotInfo* const Text::GetSlotInfo() const
{
	return m_pSlotInfo;
}

int const Text::GetNrOfCharacters() const
{
	return m_NrOfCharacters;
}

void Text::SetFont(Font font)
{
	m_pFont = font;
}

void Text::SetText(std::wstring text)
{
	m_Text = text;
	std::string convText = std::string(text.begin(), text.end());
	m_NrOfCharacters = convText.length();
}

void Text::SetPos(float2 pos)
{
	m_Pos = pos;
}

void Text::SetScale(float2 scale)
{
	m_Scale = scale;
}

void Text::SetPadding(float2 padding)
{
	m_Padding = padding;
}

void Text::SetColor(float4 color)
{
	m_Color = color;
}