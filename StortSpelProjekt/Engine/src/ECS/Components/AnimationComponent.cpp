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
	EventBus::GetInstance().Subscribe(this, &AnimationComponent::attackAnimation);
}

void component::AnimationComponent::OnUnInitScene()
{
	EventBus::GetInstance().Unsubscribe(this, &AnimationComponent::walkAnimation);
	EventBus::GetInstance().Unsubscribe(this, &AnimationComponent::attackAnimation);
}

void component::AnimationComponent::Reset()
{
	m_pAnimatedModel->ResetAnimations();
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

void component::AnimationComponent::walkAnimation(MovementInput* evnt)
{
	if (Input::GetInstance().GetKeyState(SCAN_CODES::W) || 
		Input::GetInstance().GetKeyState(SCAN_CODES::A) || 
		Input::GetInstance().GetKeyState(SCAN_CODES::S) || 
		Input::GetInstance().GetKeyState(SCAN_CODES::D)	)
	{
		m_pAnimatedModel->PlayAnimation("Run", true);
	}
	else
	{
		m_pAnimatedModel->PlayAnimation("Idle", true);
	}
}

void component::AnimationComponent::attackAnimation(MouseClick* evnt)
{
	if (evnt->button == MOUSE_BUTTON::LEFT_DOWN)
	{
		m_pAnimatedModel->PlayAnimation("Attack_Swing_Right", false);
	}
	else
	{
	}

	if (evnt->button == MOUSE_BUTTON::RIGHT_DOWN)
	{
		m_pAnimatedModel->PlayAnimation("Attack_Swing_Left", false);
	}
	else
	{
	}
}
