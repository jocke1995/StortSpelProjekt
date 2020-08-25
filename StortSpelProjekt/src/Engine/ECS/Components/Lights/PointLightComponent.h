#ifndef POINTLIGHTCOMPONENT_H
#define POINTLIGHTCOMPONENT_H

#include "Light.h"

namespace component
{
	class PointLightComponent : public Light, public Component
	{
	public:
		PointLightComponent(Entity* parent, unsigned int lightFlags = 0);
		virtual ~PointLightComponent();

		void Update(double dt);

		void SetPosition(float3 position);
		void SetAttenuation(float3 attenuation);

		void* GetLightData() const;

	private:
		PointLight* pointLight = nullptr;

		void UpdateLightData(LIGHT_COLOR_TYPE type);
	};
}

#endif