#include "stdafx.h"
#include "TransformComponent.h"
#include "../../Renderer/Transform.h"
#include "../ECS/Entity.h"

namespace component
{
	TransformComponent::TransformComponent(Entity* parent, bool invertDirection)
		:Component(parent)
	{
		m_pTransform = new Transform(invertDirection);
		m_pOriginalTransform = new Transform(invertDirection);
	}

	TransformComponent::~TransformComponent()
	{
		delete m_pTransform;
		delete m_pOriginalTransform;
	}

	void TransformComponent::Update(double dt)
	{
		m_pTransform->NormalizedMove(dt);
	}

	void TransformComponent::RenderUpdate(double dt)
	{
		m_pTransform->NormalizedMoveRender(dt);
		m_pTransform->UpdateWorldMatrix();
	}

	void TransformComponent::OnInitScene()
	{
	}

	void TransformComponent::OnUnInitScene()
	{
	}

	void TransformComponent::Reset()
	{
		*m_pTransform = *m_pOriginalTransform;
	}

	void TransformComponent::SetTransformOriginalState()
	{
		*m_pOriginalTransform = *m_pTransform;
	}

	Transform* TransformComponent::GetTransform() const
	{
		return m_pTransform;
	}
}
