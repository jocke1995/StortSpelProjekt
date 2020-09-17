#include "SandboxComponent.h"
#include "GameEntity.h"

component::SandboxComponent::SandboxComponent(Entity* parent)
	:Component(parent)
{
	m_Sandbox = "Hello there!\n";
}

component::SandboxComponent::~SandboxComponent()
{
}

void component::SandboxComponent::Update(double dt)
{
	Log::Print("General Kenobi!\n");
}

std::string component::SandboxComponent::GetSandbox()
{
	return m_Sandbox;
}
