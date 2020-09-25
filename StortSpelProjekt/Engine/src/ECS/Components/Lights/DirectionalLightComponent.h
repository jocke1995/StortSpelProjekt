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
		void SetDirection(float3 direction);
		void* GetLightData() const;
	
	protected:
		void UpdateLightIntensity();

	private:
		DirectionalLight* m_pDirectionalLight = nullptr;

		void initFlagUsages();
	};
}
#endif
