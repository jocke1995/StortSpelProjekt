#include "InputComponent.h"
#include "../Events/EventBus.h"
#include "../Entity.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/Transform.h"

component::InputComponent::InputComponent(Entity* parent)
	:Component(parent)
{
}

component::InputComponent::~InputComponent()
{
}

void component::InputComponent::InitScene(Renderer* renderer)
{
}
