#include "stdafx.h"
#include <vector>
#include "QuadManager.h"

#include "../Renderer/Texture/Texture.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Renderer.h"

#include "../Misc/Window.h"
#include "../Misc/AssetLoader.h"

#include "../Events/EventBus.h"

QuadManager::QuadManager()
{
}

QuadManager::~QuadManager()
{
	EventBus::GetInstance().Unsubscribe(this, &QuadManager::pressed);

	delete m_pQuad;
}

void QuadManager::CreateQuad(float2 pos, float2 size, bool clickable, std::wstring texturePath)
{
	if (m_pQuad != nullptr)
	{
		delete m_pQuad;
		m_Clickable = false;
	}

	if (m_pQuadTexture == nullptr && texturePath != L"NONE")
	{
		AssetLoader* al = AssetLoader::Get();
		m_pQuadTexture = al->LoadTexture2D(texturePath);
	}

	m_Clickable = clickable;

	float x = (pos.x * 2.0f) - 1.0f;
	float y = ((1.0f - pos.y) * 2.0f) - 1.0f;
	size.x = ((pos.x + size.x) * 2.0f) - 1.0f;
	size.y = ((1.0f - (pos.y + size.y)) * 2.0f) - 1.0f;

	std::vector<Vertex> m_Vertices = {};

	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3{ 1.0, 1.0, 0.0 };
	DirectX::XMFLOAT3 tangent = DirectX::XMFLOAT3{ 0.0, 0.0, 0.0 };

	Vertex vertex = {};
	vertex.pos = DirectX::XMFLOAT3{ x, y, 0.0 };
	m_Positions["upper_left"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 0.0, 0.0 };
	vertex.normal = normal;
	vertex.tangent = tangent;
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ x, size.y, 0.0 };
	m_Positions["lower_left"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 0.0, 1.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ size.x, y, 0.0 };
	m_Positions["upper_right"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 0.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ size.x, size.y, 0.0 };
	m_Positions["lower_right"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 1.0 };
	m_Vertices.push_back(vertex);

	std::vector<unsigned int> indices = { 0, 1, 2, 1, 2, 3 };

	Renderer* renderer = &Renderer::GetInstance();

	m_pQuad = new Mesh(renderer->m_pDevice5, &m_Vertices, &indices, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

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

	if (m_Clickable)
	{
		EventBus::GetInstance().Subscribe(this, &QuadManager::pressed);
	}
}

bool QuadManager::HasBeenPressed()
{
	if (m_Pressed)
	{
		m_Pressed = false;
		return true;
	}

	if (!m_Clickable)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "This quad is not clickable!\n");
	}

	return false;
}

Texture* const QuadManager::GetTexture() const
{
	return m_pQuadTexture;
}

void QuadManager::pressed(ButtonPressed* evnt)
{
	POINT p;
	GetCursorPos(&p);

	Renderer* renderer = &Renderer::GetInstance();
	ScreenToClient(*renderer->m_pWindow->GetHwnd(), &p);

	float x = (static_cast<float>(p.x) / (renderer->m_pWindow->GetScreenWidth() / 2)) - 1;
	float y = -((static_cast<float>(p.y) / (renderer->m_pWindow->GetScreenHeight() / 2)) - 1);

	if ((x >= m_Positions["upper_left"].x && y <= m_Positions["upper_left"].y)
		&& (x >= m_Positions["lower_left"].x && y >= m_Positions["lower_left"].y)
		&& (x <= m_Positions["upper_right"].x && y <= m_Positions["upper_right"].y)
		&& (x <= m_Positions["lower_right"].x && y >= m_Positions["lower_right"].y))
	{
		m_Pressed = true;
	}
}