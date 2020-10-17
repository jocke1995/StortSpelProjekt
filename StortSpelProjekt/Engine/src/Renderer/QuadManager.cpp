#include "stdafx.h"
#include "QuadManager.h"

#include "../Renderer/Texture/Texture.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Renderer.h"

#include "DX12Tasks/CopyOnDemandTask.h"

#include "../Misc/Window.h"
#include "../Misc/AssetLoader.h"

#include "../Events/EventBus.h"

QuadManager::QuadManager()
{
	m_Id = s_Id;
	s_Id++;
}

QuadManager::~QuadManager()
{
	EventBus::GetInstance().Unsubscribe(this, &QuadManager::pressed);

	if (m_pQuad != nullptr)
	{
		delete m_pQuad;
	}

	if (m_pSlotInfo != nullptr)
	{
		delete m_pSlotInfo;
	}
}

bool QuadManager::operator==(const QuadManager& other) const
{
	return (m_Id == other.m_Id);
}

void QuadManager::CreateQuad(float2 pos, float2 size, bool clickable, E_DEPTH_LEVEL depthLevel, std::wstring texturePath)
{
	// Examine if we are going to overwrite the current quad
	if (m_pQuad != nullptr)
	{
		delete m_pQuad;
		m_pQuad = nullptr;
		m_Clickable = false;

		if (m_pQuadTexture != nullptr)
		{
			delete m_pQuadTexture;
			m_pQuadTexture = nullptr;
		}
	}

	if (m_pQuadTexture == nullptr && texturePath != L"NONE")
	{
		AssetLoader* al = AssetLoader::Get();
		m_pQuadTexture = al->LoadTexture2D(texturePath);
	}

	m_Clickable = clickable;

	m_DepthLevel = depthLevel;

	float x = (pos.x * 2.0f) - 1.0f;
	float y = ((1.0f - pos.y) * 2.0f) - 1.0f;
	size.x = ((pos.x + size.x) * 2.0f) - 1.0f;
	size.y = ((1.0f - (pos.y + size.y)) * 2.0f) - 1.0f;

	std::vector<Vertex> m_Vertices = {};

	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3{ 1.0, 1.0, 0.0 };
	DirectX::XMFLOAT3 tangent = DirectX::XMFLOAT3{ 0.0, 0.0, 0.0 };

	Vertex vertex = {};
	vertex.pos = DirectX::XMFLOAT3{ x, y, 0.0f };
	m_Positions["upper_left"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 0.0, 0.0 };
	vertex.normal = normal;
	vertex.tangent = tangent;
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ x, size.y, 0.0f };
	m_Positions["lower_left"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 0.0, 1.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ size.x, y, 0.0f };
	m_Positions["upper_right"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 0.0 };
	m_Vertices.push_back(vertex);

	vertex.pos = DirectX::XMFLOAT3{ size.x, size.y, 0.0f };
	m_Positions["lower_right"] = float2{ vertex.pos.x, vertex.pos.y };
	vertex.uv = DirectX::XMFLOAT2{ 1.0, 1.0 };
	m_Vertices.push_back(vertex);

	std::vector<unsigned int> indices = { 0, 2, 1, 2, 1, 3 };

	Renderer* renderer = &Renderer::GetInstance();

	m_pQuad = new Mesh(
		renderer->m_pDevice5, 
		&m_Vertices, &indices, 
		renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV], 
		to_wstring(std::to_string(m_Id)));

	m_pQuad->Init(renderer->m_pDevice5, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

	m_pSlotInfo = new SlotInfo();
	m_pSlotInfo->vertexDataIndex = m_pQuad->GetSRV()->GetDescriptorHeapIndex();

	if (m_pQuadTexture != nullptr)
	{
		m_pSlotInfo->textureAlbedo = m_pQuadTexture->GetDescriptorHeapIndex();
	}

	if (m_Clickable)
	{
		EventBus::GetInstance().Subscribe(this, &QuadManager::pressed);
	}
}

void QuadManager::UploadAndExecuteQuadData()
{
	Renderer* renderer = &Renderer::GetInstance();

	uploadQuadData(renderer);

	renderer->executeCopyOnDemand();
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

Mesh* const QuadManager::GetQuad() const
{
	return m_pQuad;
}

Texture* const QuadManager::GetTexture() const
{
	return m_pQuadTexture;
}

SlotInfo* const QuadManager::GetSlotInfo() const
{
	return m_pSlotInfo;
}

const E_DEPTH_LEVEL* QuadManager::GetDepthLevel() const
{
	return &m_DepthLevel;
}

void QuadManager::pressed(MouseClick* evnt)
{
	if (evnt->button == MOUSE_BUTTON::LEFT_DOWN && evnt->pressed == true)
	{
		float x = 0, y = 0;
		Renderer* renderer = &Renderer::GetInstance();
		renderer->m_pWindow->MouseToScreenspace(&x, &y);

		if ((x >= m_Positions["upper_left"].x && y <= m_Positions["upper_left"].y)
			&& (x >= m_Positions["lower_left"].x && y >= m_Positions["lower_left"].y)
			&& (x <= m_Positions["upper_right"].x && y <= m_Positions["upper_right"].y)
			&& (x <= m_Positions["lower_right"].x && y >= m_Positions["lower_right"].y))
		{
			m_Pressed = true;
		}
	}
}

void QuadManager::uploadQuadData(Renderer* renderer)
{
	// Submit to GPU
	renderer->submitMeshToCodt(m_pQuad);

	renderer->submitTextureToCodt(m_pQuadTexture);
}
