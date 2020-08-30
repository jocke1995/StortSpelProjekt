#include "stdafx.h"
#include "TransformComponent.h"

namespace component
{
	TransformComponent::TransformComponent(Entity* parent)
		:Component(parent)
	{
		this->transform = new Transform();
	}

	TransformComponent::~TransformComponent()
	{
		delete this->transform;
	}

	void TransformComponent::Update(double dt)
	{

		this->transform->UpdateWorldMatrix();
	}

	Transform* TransformComponent::GetTransform() const
	{
		return this->transform;
	}
}
