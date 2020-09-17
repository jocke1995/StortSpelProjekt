#include "stdafx.h"
#include "SkyboxComponent.h"

#include "../Renderer/Transform.h"
#include "../Renderer/Model.h"

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
	m_pTransform->Move(dt);
	m_pTransform->UpdateWorldMatrix();
}

void component::SkyboxComponent::SetModel(Model* model)
{
	m_Model = model;
}

Transform* component::SkyboxComponent::GetTransform() const
{
	return m_pTransform;
}

Mesh* component::SkyboxComponent::GetMeshAt(unsigned int index) const
{
	return m_Model->GetMeshAt(index);
}

std::map<TEXTURE2D_TYPE, Texture*>* component::SkyboxComponent::GetTexturesAt(unsigned int index) const
{
	return m_Model->GetTexturesAt(index);
}

SlotInfo* component::SkyboxComponent::GetSlotInfoAt(unsigned int index) const
{
	return m_Model->GetSlotInfoAt(index);
}

unsigned int component::SkyboxComponent::GetNrOfMeshes() const
{
	return m_Model->GetSize();
}
