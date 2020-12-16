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
		void OnInitScene();
		void OnUnInitScene();

		// Set functions which modifies the shadowCamera
		void SetCameraDistance(float distance);
		void SetDirection(float3 direction);
		void SetCameraLeft(float left);
		void SetCameraRight(float right);
		void SetCameraBot(float bot);
		void SetCameraTop(float top);
		void SetCameraNearZ(float nearPlaneDistance);
		void SetCameraFarZ(float farPlaneDistance);

		void* GetLightData() const;
	
	protected:
		void UpdateLightColorIntensity();

	private:
		float m_Distance = 30.0f;
		DirectionalLight* m_pDirectionalLight = nullptr;

		void initFlagUsages();
	};
}
#endif
