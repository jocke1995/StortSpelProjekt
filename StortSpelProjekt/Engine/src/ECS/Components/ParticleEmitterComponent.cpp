#include "stdafx.h"
#include "ParticleEmitterComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"

#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Renderer.h"

#include "../Misc/AssetLoader.h"

component::ParticleEmitterComponent::ParticleEmitterComponent(Entity* parent)
	:Component(parent)
{
	// Set default mesh and texture
	AssetLoader* al = AssetLoader::Get();
	m_pMesh = al->LoadModel(L"../Vendor/Resources/Models/Quad/NormalizedQuad.obj")->GetMeshAt(0);
	m_pTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/particle0.png"));
}

component::ParticleEmitterComponent::~ParticleEmitterComponent()
{
}

void component::ParticleEmitterComponent::RenderUpdate(double dt)
{
}

void component::ParticleEmitterComponent::OnInitScene()
{
}

void component::ParticleEmitterComponent::OnUnInitScene()
{
}

void component::ParticleEmitterComponent::SetMesh(Mesh* mesh)
{
	m_pMesh = mesh;
}

void component::ParticleEmitterComponent::SetTexture(Texture2DGUI* texture)
{
	m_pTexture = texture;
}

Mesh* component::ParticleEmitterComponent::GetMesh() const
{
	return m_pMesh;
}

Texture2DGUI* component::ParticleEmitterComponent::GetTexture() const
{
	return m_pTexture;
}
