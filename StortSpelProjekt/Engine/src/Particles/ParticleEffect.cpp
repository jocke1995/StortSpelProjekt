#include "stdafx.h"
#include "ParticleEffect.h"

#include "../Misc/AssetLoader.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/DescriptorHeap.h"

#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/GPUMemory/UnorderedAccessView.h"

#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Model.h"
#include "../Renderer/Mesh.h"

EngineRand ParticleEffect::rand = {};

ParticleEffect::ParticleEffect(std::wstring name, DescriptorHeap* descriptorHeap, Texture2DGUI* texture, unsigned int particleCount)
{
	m_ParticleCount = particleCount;

	init(name, descriptorHeap, texture);
}

ParticleEffect::~ParticleEffect()
{
	delete m_pUploadResource;
	delete m_pDefaultResource;
	delete m_pSRV;
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

void ParticleEffect::init(std::wstring name, DescriptorHeap* descriptorHeap, Texture2DGUI* texture)
{
	m_Name = name;
	m_pTexture = texture;

	if (texture == nullptr)
	{
		// Set default texture
		AssetLoader* al = AssetLoader::Get();
		m_pTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/minimap.png"));

		Log::PrintSeverity(Log::Severity::WARNING, "ParticleEffect::Texture was nullptr, %S\n", name.c_str());
	}

	Renderer& renderer = Renderer::GetInstance();

	// Only send position (float3) + size (float) to gpu
	size_t entrySize = sizeof(PARTICLE_DATA);
	unsigned long long resourceByteSize = entrySize * m_ParticleCount;

	// used to format a debug string
	std::wstring a = L"ParticleEffect_";
	std::wstring b = name;
	std::wstring c = L"_UPLOAD";
	std::wstring d = L"_DEFAULT";

	// Resources
	m_pUploadResource = new Resource(renderer.m_pDevice5, resourceByteSize, RESOURCE_TYPE::UPLOAD, a + b + c);
	m_pDefaultResource = new Resource(renderer.m_pDevice5, resourceByteSize, RESOURCE_TYPE::DEFAULT, a + b + d);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_ParticleCount;
	srvDesc.Buffer.StructureByteStride = entrySize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	m_pSRV = new ShaderResourceView(renderer.m_pDevice5, descriptorHeap, &srvDesc, m_pDefaultResource);


	m_Particles.reserve(m_ParticleCount);
	m_ParticlesData.resize(m_ParticleCount);

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
	float x = rand.Randf(0, 70) -35;
	float z = rand.Randf(0, 70) -35;

	particle.m_Position.x = x;
	particle.m_Position.z = z;
}

void ParticleEffect::randomizeVelocity(Particle& particle)
{
}

void ParticleEffect::randomizeSize(Particle& particle)
{
	float size = rand.Randf(0, 20);

	particle.m_Size = size;
}

void ParticleEffect::randomizeRotation(Particle& particle)
{
}

void ParticleEffect::randomizeLifetime(Particle& particle)
{
	particle.m_Lifetime = 2;
}

void ParticleEffect::updateResourceData()
{
	PARTICLE_DATA tempData;

	unsigned int index = 0;
	for (Particle& p : m_Particles)
	{
#ifdef PARTICLESYSTEM_RENDER_DEAD_PARTICLES
		if (!p.IsAlive())
		{
			p.m_Size = 0;
		}
#endif // PARTICLESYSTEM_RENDER_DEAD_PARTICLES

		tempData = { p.m_Position.x, p.m_Position.y, p.m_Position.z, p.m_Size };
		
		m_ParticlesData[index++] = tempData;
	}

	// Todo, sort z for blend

	const void* data = static_cast<void*>(m_ParticlesData.data());
	std::tuple temp = { m_pUploadResource, m_pDefaultResource, data };
	
	// Copy to ondemand
	Renderer& renderer = Renderer::GetInstance();
	renderer.submitToCodt(&temp);
}
