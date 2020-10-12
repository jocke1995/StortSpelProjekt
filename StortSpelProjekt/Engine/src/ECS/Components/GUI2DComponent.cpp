#include "stdafx.h"
#include <vector>
#include "GUI2DComponent.h"

#include "../Renderer/Texture/Texture.h"
#include "../Renderer/Texture/Texture2D.h"
#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/Mesh.h"

#include "../Misc/Window.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/GUI2DElements/Text.h"
#include "../Misc/GUI2DElements/Font.h"

namespace component
{
	GUI2DComponent::GUI2DComponent(Entity* parent)
		:Component(parent)
	{
	}

	GUI2DComponent::~GUI2DComponent()
	{
		for (auto textMap : m_TextMap)
		{
			delete textMap.second;
		}

		m_TextMap.clear();
	}

	std::map<std::string, TextData>* const GUI2DComponent::GetTextDataMap()
	{
		return &m_TextDataMap;
	}

	TextData* GUI2DComponent::GetTextData(std::string name)
	{
		return &m_TextDataMap[name];
	}

	void GUI2DComponent::AddText(std::string name)
	{
		// Look if a font is chosen, otherwise, set the default font
		if (m_pFont == nullptr)
		{
			m_pFont = AssetLoader::Get()->LoadFontFromFile(L"Arial.fnt");
		}

		// Default text
		TextData textData = {};
		textData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		textData.padding = { 0.5f, 0.0f };
		textData.pos = { 0.5f, 0.5f };
		textData.scale = { 1.0f, 1.0f };
		textData.text = L"DEFAULT_TEXT";

		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				Log::PrintSeverity(Log::Severity::WARNING, "It already exists a text with the name '%s'! Overwriting text data...\n", name.c_str());
			}
		}

		m_TextDataMap[name] = textData;
	}

	void GUI2DComponent::SubmitText(Text* text, std::string name)
	{
		m_TextMap.insert({ name, text });
	}

	void GUI2DComponent::ReplaceText(Text* text, std::string name)
	{
		bool found = false;
		for (auto textMap : m_TextMap)
		{
			if (textMap.first == name)
			{
				delete m_TextMap[name];
				m_TextMap[name] = text;
				found = true;
			}
		}

		if (found == false)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Could not find any text called '%s' to replace!\n", name);
		}
	}

	void GUI2DComponent::UploadTextData(std::string name)
	{
		Renderer::GetInstance().LoadTexture(m_pFont->GetTexture());

		int numOfCharacters = GetNumOfCharacters(name);
		auto textData = GetTextData(name);

		Renderer* renderer = &Renderer::GetInstance();

		Text* text = new Text(
			renderer->m_pDevice5,
			renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
			numOfCharacters,
			m_pFont->GetTexture());
		text->SetTextData(textData, m_pFont);

		// Look if the text exists
		bool exists = false;
		for (auto textMap : m_TextMap)
		{
			if (textMap.first == name)
			{
				exists = true;
			}
		}

		if (exists == true)
		{
			ReplaceText(text, name);
		}
		else
		{
			SubmitText(text, name);
		}

		renderer->submitTextToGPU(text, this);

		renderer->executeCopyOnDemand();
	}

	void GUI2DComponent::SetFont(Font* font)
	{
		m_pFont = font;
	}

	void GUI2DComponent::SetText(std::string text, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].text = to_wstring(text);
				exists = true;
				break;
			}
		}
		
		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set text.\n", name.c_str());
		}
	}

	void GUI2DComponent::SetPos(float2 textPos, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].pos = textPos;
				exists = true;
				break;
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set position.\n", name.c_str());
		}
	}

	void GUI2DComponent::SetScale(float2 scale, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				// Scale with the size of the window
				Renderer* renderer = &Renderer::GetInstance();			
				HWND* hwnd = const_cast<HWND*>(renderer->m_pWindow->GetHwnd());
				RECT rect;

				float win_x = 0, win_y = 0;
				if (GetWindowRect(*hwnd, &rect))
				{
					win_x = rect.right - rect.left;
					win_y = rect.bottom - rect.top;
				}

				float scale_x = 0, scale_y = 0;
				scale_x = win_x / 1000;
				scale_y = win_y / 1000;
				float aspect = scale_x / scale_y;

				m_TextDataMap[name].scale.x = (scale.x * scale_x);
				m_TextDataMap[name].scale.y = (scale.y * scale_y * aspect);

				exists = true;
				break;
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set scale.\n", name.c_str());
		}
	}

	void GUI2DComponent::SetPadding(float2 padding, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].padding = padding;
				exists = true;
				break;
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set padding.\n", name.c_str());
		}
	}

	void GUI2DComponent::SetColor(float4 color, std::string name)
	{
		bool exists = false;
		for (auto data : m_TextDataMap)
		{
			if (data.first == name)
			{
				m_TextDataMap[name].color = color;
				exists = true;
				break;
			}
		}

		if (exists == false)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set color.\n", name.c_str());
		}
	}

	Font* GUI2DComponent::GetFont() const
	{
		return m_pFont;
	}

	Texture* GUI2DComponent::GetFontTexture() const
	{
		return m_pFont->GetTexture();
	}

	Text* GUI2DComponent::GetText(std::string name)
	{
 		return m_TextMap[name];
	}

	std::map<std::string, Text*>* GUI2DComponent::GetTextMap()
	{
		return &m_TextMap;
	}

	const int GUI2DComponent::GetNumOfTexts() const
	{
		return m_TextMap.size();
	}

	const int GUI2DComponent::GetNumOfCharacters(std::string name)
	{
		return m_TextDataMap[name].text.size();
	}

	void GUI2DComponent::CreateQuad(float2 pos, float2 scale, std::wstring path)
	{
		if (m_pQuad != nullptr)
		{
			delete m_pQuad;
			delete m_pQuadTexture;
		}

		if (m_pQuadTexture == nullptr && path != L"NONE")
		{
			m_pQuadTexture = new Texture2DGUI(path);
		}

		std::vector<Vertex> vertices = {};

		float x = (pos.x * 2.0f) - 1.0f;
		float y = ((1.0f - pos.y) * 2.0f) - 1.0f;

		DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3{ 1.0, 1.0, 0.0 };
		DirectX::XMFLOAT3 tangent = DirectX::XMFLOAT3{ 0.0, 0.0, 0.0 };

		Vertex vertex = {};
		vertex.pos = DirectX::XMFLOAT3{ x, y, 0.0 };
		vertex.uv = DirectX::XMFLOAT2{ 0.0, 0.0 };
		vertex.normal = normal;
		vertex.tangent = tangent;
		vertices.push_back(vertex);

		vertex.pos = DirectX::XMFLOAT3{ x, y + scale.y, 0.0 };
		vertex.uv = DirectX::XMFLOAT2{ 0.0, 1.0 };
		vertices.push_back(vertex);

		vertex.pos = DirectX::XMFLOAT3{ x + scale.x, y, 0.0 };
		vertex.uv = DirectX::XMFLOAT2{ 1.0, 0.0 };
		vertices.push_back(vertex);

		vertex.pos = DirectX::XMFLOAT3{ x, y + scale.y, 0.0 };
		vertex.uv = DirectX::XMFLOAT2{ 0.0, 1.0 };
		vertices.push_back(vertex);

		vertex.pos = DirectX::XMFLOAT3{ x + scale.x, y + scale.y, 0.0 };
		vertex.uv = DirectX::XMFLOAT2{ 1.0, 1.0 };
		vertices.push_back(vertex);

		std::vector<unsigned int> indices = { 0, 1, 2, 1, 2, 3 };

		Renderer* renderer = &Renderer::GetInstance();

		m_pQuad = new Mesh(renderer->m_pDevice5, &vertices, &indices, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

		m_pQuad->m_pUploadResourceVertices = new Resource(renderer->m_pDevice5, m_pQuad->GetSizeOfVertices(), RESOURCE_TYPE::UPLOAD, L"Vertex_UPLOAD_RESOURCE");
		m_pQuad->m_pDefaultResourceVertices = new Resource(renderer->m_pDevice5, m_pQuad->GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"Vertex_DEFAULT_RESOURCE");

		// Vertices
		const void* data = static_cast<const void*>(m_pQuad->m_Vertices.data());
		Resource* uploadR = m_pQuad->m_pUploadResourceVertices;
		Resource* defaultR = m_pQuad->m_pDefaultResourceVertices;

		// Create SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
		dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		dsrv.Buffer.FirstElement = 0;
		dsrv.Format = DXGI_FORMAT_UNKNOWN;
		dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		dsrv.Buffer.NumElements = m_pQuad->GetNumVertices();
		dsrv.Buffer.StructureByteStride = sizeof(Vertex);

		m_pQuad->m_pSRV = new ShaderResourceView(
			renderer->m_pDevice5,
			renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
			&dsrv,
			m_pQuad->m_pDefaultResourceVertices);

		// Set indices resource
		m_pQuad->m_pUploadResourceIndices = new Resource(renderer->m_pDevice5, m_pQuad->GetSizeOfIndices(), RESOURCE_TYPE::UPLOAD, L"Index_UPLOAD_RESOURCE");
		m_pQuad->m_pDefaultResourceIndices = new Resource(renderer->m_pDevice5, m_pQuad->GetSizeOfIndices(), RESOURCE_TYPE::DEFAULT, L"Index_DEFAULT_RESOURCE");

		// inidices
		data = static_cast<const void*>(m_pQuad->m_Indices.data());
		uploadR = m_pQuad->m_pUploadResourceIndices;
		defaultR = m_pQuad->m_pDefaultResourceIndices;

		// Set indexBufferView
		m_pQuad->m_pIndexBufferView = new D3D12_INDEX_BUFFER_VIEW();
		m_pQuad->m_pIndexBufferView->BufferLocation = m_pQuad->m_pDefaultResourceIndices->GetGPUVirtualAdress();
		m_pQuad->m_pIndexBufferView->Format = DXGI_FORMAT_R32_UINT;
		m_pQuad->m_pIndexBufferView->SizeInBytes = m_pQuad->GetSizeOfIndices();
	}

	void GUI2DComponent::Update(double dt)
	{
	}

	void GUI2DComponent::OnInitScene()
	{
		Renderer::GetInstance().InitGUI2DComponent(GetParent());
	}

	void GUI2DComponent::OnLoadScene()
	{
		Renderer::GetInstance().LoadTexture(m_pFont->GetTexture());
	}

	void GUI2DComponent::OnUnloadScene()
	{
		Renderer::GetInstance().UnloadTexture(m_pFont->GetTexture());
	}
}
