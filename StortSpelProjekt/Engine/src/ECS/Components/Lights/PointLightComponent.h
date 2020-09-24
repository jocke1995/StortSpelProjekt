#ifndef POINTLIGHTCOMPONENT_H
#define POINTLIGHTCOMPONENT_H

#include "Light.h"
#include "../Component.h"

namespace component
{
	class PointLightComponent : public Light, public Component
	{
	public:
		PointLightComponent(Entity* parent, unsigned int lightFlags = 0);
		virtual ~PointLightComponent();

		void Update(double dt);
		void InitScene(Renderer* renderer);

		void SetPosition(float3 position);
		void SetAttenuation(float3 attenuation);

		void* GetLightData() const;

	protected:
		void UpdateLightData(COLOR_TYPE type);

	private:
		PointLight* m_pPointLight = nullptr;
	};
}

#endif