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

#include <algorithm>

EngineRand ParticleEffect::rand = {};


ParticleEffect::ParticleEffect(DescriptorHeap* descriptorHeap, Texture2DGUI* texture, ParticleEffectSettings* settings)
{
	m_pTexture = texture;
	m_Settings = *settings;

	init(descriptorHeap);
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
	while (m_TimeSinceSpawn >= m_Settings.spawnInterval)
	{
		spawnParticle();

		m_TimeSinceSpawn -= m_Settings.spawnInterval;
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

Texture2DGUI* ParticleEffect::GetTexture() const
{
	return m_pTexture;
}

bool ParticleEffect::spawnParticle()
{
	// m_ParticleIndex is always at the oldest particle first
	Particle& particle = m_Particles.at(m_ParticleIndex);

	// If particle is alive, wait (don't spawn yet / continue)
	if (particle.IsAlive())
	{
		return false;
	}

	// Update ParticleIndex
	m_ParticleIndex = ++m_ParticleIndex % m_Settings.particleCount;

	// "Spawn"
	initParticle(particle);

	return true;
}

void ParticleEffect::init(DescriptorHeap* descriptorHeap)
{
	if (m_pTexture == nullptr)
	{
		// Set default texture
		AssetLoader* al = AssetLoader::Get();
		m_pTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/stefanHuvud.png"));

		Log::PrintSeverity(Log::Severity::WARNING, "ParticleEffect::Texture was nullptr\n");
	}

	Renderer& renderer = Renderer::GetInstance();

	// Only send position (float3) + size (float) to gpu
	size_t entrySize = sizeof(PARTICLE_DATA);
	unsigned long long resourceByteSize = entrySize * m_Settings.particleCount;

	// used to format a debug string
	std::wstring a = L"ParticleEffect_";
	static unsigned int particleCounter = 0;
	std::wstring b = std::to_wstring(particleCounter++);
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
	srvDesc.Buffer.NumElements = m_Settings.particleCount;
	srvDesc.Buffer.StructureByteStride = entrySize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	m_pSRV = new ShaderResourceView(renderer.m_pDevice5, descriptorHeap, &srvDesc, m_pDefaultResource);

	m_Particles.resize(m_Settings.particleCount);
	m_ParticlesData.resize(m_Settings.particleCount);
}

void ParticleEffect::initParticle(Particle& particle)
{
	// Set start values
	particle.initDefaultValues(&m_Settings.startValues);

	randomizePosition(particle);
	randomizeVelocity(particle);
	randomizeSize(particle);
	randomizeRotation(particle);
	randomizeRotationSpeed(particle);
	randomizeLifetime(particle);
}

void ParticleEffect::randomizePosition(Particle& particle)
{
	RandomParameter3 randParams = m_Settings.randPosition;
	
	if (randParams.x.interval.x != 0 && randParams.x.interval.y != 0)
	{
		float x = rand.Randf(randParams.x.interval.x, randParams.x.interval.y);
		particle.m_Attributes.position.x = x;
	}

	if (randParams.y.interval.x != 0 && randParams.y.interval.y != 0)
	{
		float y = rand.Randf(randParams.y.interval.x, randParams.y.interval.y);
		particle.m_Attributes.position.y = y;
	}

	if (randParams.z.interval.x != 0 && randParams.z.interval.y != 0)
	{
		float z = rand.Randf(randParams.z.interval.x, randParams.z.interval.y);
		particle.m_Attributes.position.z = z;
	}
}

void ParticleEffect::randomizeVelocity(Particle& particle)
{
	RandomParameter3 randParams = m_Settings.randVelocity;

	if (randParams.x.interval.x != 0 && randParams.x.interval.y != 0)
	{
		float x = rand.Randf(randParams.x.interval.x, randParams.x.interval.y);
		particle.m_Attributes.velocity.x = x;
	}

	if (randParams.y.interval.x != 0 && randParams.y.interval.y != 0)
	{
		float y = rand.Randf(randParams.y.interval.x, randParams.y.interval.y);
		particle.m_Attributes.velocity.y = y;
	}

	if (randParams.z.interval.x != 0 && randParams.z.interval.y != 0)
	{
		float z = rand.Randf(randParams.z.interval.x, randParams.z.interval.y);
		particle.m_Attributes.velocity.z = z;
	}
}

void ParticleEffect::randomizeSize(Particle& particle)
{
	RandomParameter randParam = m_Settings.randSize;

	if (randParam.interval.x != 0 && randParam.interval.y != 0)
	{
		float size = rand.Randf(randParam.interval.x, randParam.interval.y);
		particle.m_Attributes.size = size;
	}
}

void ParticleEffect::randomizeRotation(Particle& particle)
{
	RandomParameter randParam = m_Settings.randRotation;

	if (randParam.interval.x != 0 && randParam.interval.y != 0)
	{
		float rot = rand.Randf(randParam.interval.x, randParam.interval.y);
		particle.m_Attributes.rotation = rot;
	}
}

void ParticleEffect::randomizeRotationSpeed(Particle& particle)
{
	RandomParameter randParam = m_Settings.randRotationSpeed;
	
	if (randParam.interval.x != 0 && randParam.interval.y != 0)
	{
		float rotSpeed = rand.Randf(randParam.interval.x, randParam.interval.y);
		particle.m_Attributes.rotationSpeed = rotSpeed;
	}
}

void ParticleEffect::randomizeLifetime(Particle& particle)
{
	RandomParameter randParam = m_Settings.randLifetime;
	
	if (randParam.interval.x != 0 && randParam.interval.y != 0)
	{
		float lifetime = rand.Randf(randParam.interval.x, randParam.interval.y);
		particle.m_Attributes.lifetime = lifetime;
	}
}

void ParticleEffect::updateResourceData(float3 cameraPos)
{
	PARTICLE_DATA tempData;

	unsigned int index = 0;
	for (Particle& p : m_Particles)
	{
#ifdef PARTICLESYSTEM_RENDER_DEAD_PARTICLES
		if (!p.IsAlive())
		{
			p.m_Attributes.size = 0;
		}
#endif // PARTICLESYSTEM_RENDER_DEAD_PARTICLES

		tempData = { p.m_Attributes.position.x, p.m_Attributes.position.y, p.m_Attributes.position.z, p.m_Attributes.size, p.m_Attributes.rotation };
		
		m_ParticlesData[index++] = tempData;
	}

	// Sort z from camera for blend
	std::sort(m_ParticlesData.begin(), m_ParticlesData.end(),
		[&](const PARTICLE_DATA& i, const PARTICLE_DATA& j) -> bool 
	{
		float distFromCamI = (i.position - cameraPos).length();
		float distFromCamJ = (j.position - cameraPos).length();

		return distFromCamI > distFromCamJ;
	});

	const void* data = static_cast<void*>(m_ParticlesData.data());
	std::tuple temp = { m_pUploadResource, m_pDefaultResource, data };
	
	// Copy to ondemand
	Renderer& renderer = Renderer::GetInstance();
	renderer.submitToCodt(&temp);
}
