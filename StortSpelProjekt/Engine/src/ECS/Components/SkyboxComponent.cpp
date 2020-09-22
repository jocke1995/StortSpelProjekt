#include "stdafx.h"
#include "SkyboxComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"
#include "../Renderer/BaseCamera.h"

component::SkyboxComponent::SkyboxComponent(Entity* parent)
	:Component(parent)
{
	m_pTransform = new Transform();
}

component::SkyboxComponent::~SkyboxComponent()
{
	delete m_pTransform;
}

void component::SkyboxComponent::Update(double dt)
{
	m_pTransform->SetPosition(m_Camera->GetPosition());
	m_pTransform->UpdateWorldMatrix();
}

void component::SkyboxComponent::SetMesh(Mesh* mesh)
{
	m_Mesh = mesh;
}

void component::SkyboxComponent::SetTexture(Texture* texture)
{
	m_Texture = texture;
}

void component::SkyboxComponent::SetCamera(BaseCamera* camera)
{
	m_Camera = camera;
}

Transform* component::SkyboxComponent::GetTransform() const
{
	return m_pTransform;
}

Mesh* component::SkyboxComponent::GetMesh() const
{
	return m_Mesh;
}

Texture* component::SkyboxComponent::GetTexture() const
{
	return m_Texture;
}