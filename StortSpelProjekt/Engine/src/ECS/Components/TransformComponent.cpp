#include "stdafx.h"
#include "TransformComponent.h"
#include "../../Renderer/Transform.h"
#include "../ECS/Entity.h"

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
		m_pTransform->Move(dt);
	}

	void TransformComponent::RenderUpdate(double dt)
	{
		m_pTransform->MoveRender(dt);
		m_pTransform->UpdateWorldMatrix();
	}

	void TransformComponent::InitScene()
	{
		Log::Print("TransformComponent InitScene called!\n");
	}

	Transform* TransformComponent::GetTransform() const
	{
		return m_pTransform;
	}
}
