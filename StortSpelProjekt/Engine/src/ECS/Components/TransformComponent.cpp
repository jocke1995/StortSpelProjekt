#include "stdafx.h"
#include "TransformComponent.h"
#include "../../Renderer/Transform.h"

namespace component
{
	TransformComponent::TransformComponent(Entity* parent)
		:Component(parent)
	{
		m_pTransform = new Transform();
	}

	TransformComponent::~TransformComponent()
	{
		delete m_pTransform;
	}

	void TransformComponent::Update(double dt)
	{

		m_pTransform->UpdateWorldMatrix();
	}

	Transform* TransformComponent::GetTransform() const
	{
		return m_pTransform;
	}
}
