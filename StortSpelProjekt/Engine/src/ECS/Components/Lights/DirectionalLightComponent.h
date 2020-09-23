#ifndef DIRECTIONALLIGHTCOMPONENT_H
#define DIRECTIONALLIGHTCOMPONENT_H

#include "Light.h"
#include "../Component.h"

namespace component
{
	class DirectionalLightComponent :  public Light, public Component
	{
	public:
		DirectionalLightComponent(Entity* parent, unsigned int lightFlags = 0);
		virtual ~DirectionalLightComponent();

		void Update(double dt);
		void Init(Renderer* renderer);
		void SetDirection(float3 direction);
		void* GetLightData() const;
	
	protected:
		void UpdateLightData(COLOR_TYPE type);

	private:
		DirectionalLight* directionalLight = nullptr;

		void initFlagUsages();

	};
}
#endif
