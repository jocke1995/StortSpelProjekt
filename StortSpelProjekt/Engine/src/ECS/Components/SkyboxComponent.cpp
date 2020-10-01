#include "stdafx.h"
#include "SkyboxComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"
#include "../Renderer/BaseCamera.h"

#include "../Renderer/Texture/TextureCubeMap.h"
#include "../Renderer/Renderer.h"

#include "../Misc/AssetLoader.h"

component::SkyboxComponent::SkyboxComponent(Entity* parent)
	:Component(parent)
{
	m_pTransform = new Transform();
	
	// Set default mesh and texture
	AssetLoader* al = AssetLoader::Get();
	m_pMesh = al->LoadModel(L"../Vendor/Resources/Models/Cube/cube.obj")->GetMeshAt(0);
	m_pTexture = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
}

component::SkyboxComponent::~SkyboxComponent()
{
	delete m_pTransform;
}

void component::SkyboxComponent::RenderUpdate(double dt)
{
	
}

void component::SkyboxComponent::InitScene()
{
	Renderer::GetInstance().InitSkyboxComponent(GetParent());
}

void component::SkyboxComponent::SetMesh(Mesh* mesh)
{
	m_pMesh = mesh;
}

void component::SkyboxComponent::SetTexture(TextureCubeMap* texture)
{
	if (texture->GetType() != TEXTURE_TYPE::TEXTURECUBEMAP)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "SkyboxComponent: Texture needs to be of type TEXTURE_TYPE::TEXTURECUBEMAP");
	}
	m_pTexture = texture;
}

void component::SkyboxComponent::SetCamera(BaseCamera* camera)
{
	m_pCamera = camera;
}

Transform* component::SkyboxComponent::GetTransform() const
{
	return m_pTransform;
}

Mesh* component::SkyboxComponent::GetMesh() const
{
	return m_pMesh;
}

TextureCubeMap* component::SkyboxComponent::GetTexture() const
{
	return m_pTexture;
}

BaseCamera* component::SkyboxComponent::GetCamera() const
{
	return m_pCamera;
}
