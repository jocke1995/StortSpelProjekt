#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H
#include "EngineMath.h"
#include "Component.h"
#include "Core.h"
class BaseCamera;
class ModifierInput;
class MouseScroll;
class MovementInput;
class MouseMovement;

namespace component
{
	class InputComponent : public Component
	{
	public:
		// Default Settings
		InputComponent(Entity* parent);

		virtual ~InputComponent();

		virtual void Init() = 0;

		virtual void Update(double dt) = 0;

	};
}

#endif