#include "AnimationComponent.h"

#include "../Renderer/AnimatedModel.h"
#include "../Events/EventBus.h"
#include "../ECS/Entity.h"

component::AnimationComponent::AnimationComponent(Entity* parent)
	:Component(parent)
{
	EventBus::GetInstance().Subscribe(this, &AnimationComponent::walkAnimation);
}

component::AnimationComponent::~AnimationComponent()
{
}

void component::AnimationComponent::RenderUpdate(double dt)
{
	m_pModel->Update(dt);
}

void component::AnimationComponent::OnInitScene()
{
}

void component::AnimationComponent::OnUnInitScene()
{
}

void component::AnimationComponent::Initialize()
{
	component::ModelComponent* mc = m_pParent->GetComponent<component::ModelComponent>();
	if (mc != nullptr)
	{
		m_pModel = dynamic_cast<AnimatedModel*>(mc->GetModel());
		if (m_pModel == nullptr)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "AnimationComponent initialized when there's no animated model! Make sure the model used actually has animations.\n");
		}
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "AnimationComponent initialized when there's no ModelComponent! Make sure the AnimationComponent is initialized AFTER the modelComponent.\n");
	}
}

void component::AnimationComponent::SetActiveAnimation(std::string animationName)
{
	m_pModel->SetActiveAnimation(animationName);
}

void component::AnimationComponent::walkAnimation(MovementInput* evnt)
{
	if (evnt->key == SCAN_CODES::W && evnt->pressed)
	{
		m_pModel->toggleAnimation();	// Play on movement.
	}
	else if (evnt->key == SCAN_CODES::W && !evnt->pressed)
	{
		m_pModel->toggleAnimation();	// Pause on no movement.
	}
}
