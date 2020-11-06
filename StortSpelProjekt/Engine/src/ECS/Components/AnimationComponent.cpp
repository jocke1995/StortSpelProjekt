#include "AnimationComponent.h"

#include "../Renderer/AnimatedModel.h"
#include "../Events/EventBus.h"
#include "../ECS/Entity.h"

component::AnimationComponent::AnimationComponent(Entity* parent)
	:Component(parent)
{
}

component::AnimationComponent::~AnimationComponent()
{
}

void component::AnimationComponent::RenderUpdate(double dt)
{
	m_pAnimatedModel->Update(dt);
}

void component::AnimationComponent::OnInitScene()
{
	EventBus::GetInstance().Subscribe(this, &AnimationComponent::walkAnimation);
}

void component::AnimationComponent::OnUnInitScene()
{
	EventBus::GetInstance().Unsubscribe(this, &AnimationComponent::walkAnimation);
}

void component::AnimationComponent::Initialize()
{
	component::ModelComponent* mc = m_pParent->GetComponent<component::ModelComponent>();
	if (mc != nullptr)
	{
		m_pAnimatedModel = dynamic_cast<AnimatedModel*>(mc->GetModel());
		if (m_pAnimatedModel == nullptr)
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
	m_pAnimatedModel->SetActiveAnimation(animationName);
}

void component::AnimationComponent::walkAnimation(MovementInput* evnt)
{
	if (	Input::GetInstance().GetKeyState(SCAN_CODES::W) || 
			Input::GetInstance().GetKeyState(SCAN_CODES::A) ||
			Input::GetInstance().GetKeyState(SCAN_CODES::S) || 
			Input::GetInstance().GetKeyState(SCAN_CODES::D))
	{
		m_pAnimatedModel->PlayAnimation();	// Play on movement.
	}
	else
	{
		m_pAnimatedModel->PauseAnimation();	// Pause on no movement.
		m_pAnimatedModel->ResetAnimation();
	}
}
