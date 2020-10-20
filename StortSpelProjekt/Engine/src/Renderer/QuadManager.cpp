#include "stdafx.h"
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
}

bool QuadManager::operator==(const QuadManager& other) const
{
	return (m_Id == other.m_Id);
}

void QuadManager::CreateQuad(float2 pos, float2 size, bool clickable, std::wstring texturePath)
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

	m_pQuad = new Mesh(
		&m_Vertices, &indices,
		to_wstring(std::to_string(m_Id)));

	m_pQuad->Init(renderer->m_pDevice5, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);

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

void QuadManager::pressed(MouseClick* evnt)
{
	if (evnt->button == MOUSE_BUTTON::LEFT_DOWN && evnt->pressed == true)
	{
		int x = 0, y = 0;
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