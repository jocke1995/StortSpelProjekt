#include "stdafx.h"
#include "ParticleEffect.h"

#include "../Misc/AssetLoader.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/DescriptorHeap.h"

#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/GPUMemory/UnorderedAccessView.h"

#include "../Renderer/Texture/Texture2DGUI.h"

EngineRand ParticleEffect::rand = {};

ParticleEffect::ParticleEffect(std::wstring name, DescriptorHeap* descriptorHeap)
{
	m_ParticleCount = 5;

	init(name, descriptorHeap);
}

ParticleEffect::ParticleEffect(std::wstring name, DescriptorHeap* descriptorHeap, unsigned int particleCount)
{
	m_ParticleCount = particleCount;

	init(name, descriptorHeap);
}

ParticleEffect::~ParticleEffect()
{
	delete m_pUploadResource;
	delete m_pUAVResource;
	delete m_pSRV;
	delete m_pUAV;
}

void ParticleEffect::Update(double dt)
{
	m_TimeSinceSpawn += dt;
	
	// spawn particles
	if (m_TimeSinceSpawn >= m_SpawnInterval)
	{
		spawnParticle();

		m_TimeSinceSpawn = 0;
	}

	// Update all particles
	for (Particle& particle : m_Particles)
	{
		// Only update if alive
		if (particle.IsAlive())
		{
			particle.Update(dt);
		}
	}
}

const std::wstring& ParticleEffect::GetName() const
{
	return m_Name;
}

Texture2DGUI* ParticleEffect::GetTexture() const
{
	return m_pTexture;
}

void ParticleEffect::spawnParticle()
{
	// m_ParticleIndex is always at the oldest particle first
	Particle& particle = m_Particles.at(m_ParticleIndex);

	// If particle is alive, wait (don't spawn yet / continue)
	if (particle.IsAlive())
	{
		return;
	}

	// Update ParticleIndex
	m_ParticleIndex = ++m_ParticleIndex % m_ParticleCount;

	// "Spawn"
	initParticle(particle);
}

void ParticleEffect::init(std::wstring name, DescriptorHeap* descriptorHeap)
{
	m_Name = name;

	// Set default mesh and texture
	AssetLoader* al = AssetLoader::Get();
	m_pTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/particle0.png"));

	Renderer& renderer = Renderer::GetInstance();

	// Only send position (float3) + size (float) to gpu
	size_t particleEntrySize = sizeof(float4);
	unsigned long long resourceByteSize = particleEntrySize * m_ParticleCount;

	m_pUploadResource = new Resource(renderer.m_pDevice5, resourceByteSize, RESOURCE_TYPE::UPLOAD, L"ParticleEffect_UPLOAD");

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_ParticleCount;
	srvDesc.Buffer.StructureByteStride = particleEntrySize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	m_pSRV = new ShaderResourceView(renderer.m_pDevice5, descriptorHeap, &srvDesc, m_pUploadResource);

	// GPU writes to this resource, Billboarding creates worldMatrix
	size_t UAVEntrySize = sizeof(float4x4);
	resourceByteSize = UAVEntrySize * m_ParticleCount;
	m_pUAVResource = new Resource(renderer.m_pDevice5, resourceByteSize, RESOURCE_TYPE::DEFAULT, L"ParticleEffect_UAV", D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_ParticleCount;
	uavDesc.Buffer.StructureByteStride = UAVEntrySize;

	m_pUAV = new UnorderedAccessView(renderer.m_pDevice5, descriptorHeap, &uavDesc, m_pUAVResource);


	m_Particles.reserve(m_ParticleCount);

	m_Particles = std::vector<Particle>(m_ParticleCount);
}

void ParticleEffect::initParticle(Particle& particle)
{
	// Todo, later get these from some kind of preset
	particle.initDefaultValues();

	randomizePosition(particle);
	randomizeVelocity(particle);
	randomizeSize(particle);
	randomizeRotation(particle);
	randomizeLifetime(particle);
}

void ParticleEffect::randomizePosition(Particle& particle)
{
	float x = rand.Randf(-1, 1);
	float y = rand.Randf(-1, 1);
	
	particle.m_Position.x = x;
	particle.m_Position.y = y;
}

void ParticleEffect::randomizeVelocity(Particle& particle)
{
}

void ParticleEffect::randomizeSize(Particle& particle)
{
}

void ParticleEffect::randomizeRotation(Particle& particle)
{
}

void ParticleEffect::randomizeLifetime(Particle& particle)
{
	particle.m_Lifetime = 5;
}

void ParticleEffect::updateResourceData()
{
	// build a array with float3 position
	float4 pos;
	std::vector<float4> tempData(m_ParticleCount);

	unsigned int index = 0;
	for (Particle& p : m_Particles)
	{
		pos = { p.m_Position.x, p.m_Position.y, p.m_Position.z, p.m_Size };
		if (pos.y < -4000)
		{
			int a = 0;
		}
		tempData[index++] = pos;
	}

	// Todo, sort z for blend

	m_pUploadResource->SetData(tempData.data());
}
