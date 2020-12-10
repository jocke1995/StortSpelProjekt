#include "stdafx.h"
#include "PointLightComponent.h"

// Renderer
#include "../Renderer/Transform.h"
#include "../Renderer/Camera/BaseCamera.h"
#include "../Renderer/Renderer.h"

// ECS
#include "../ECS/Entity.h"

namespace component
{
	PointLightComponent::PointLightComponent(Entity* parent, unsigned int lightFlags)
		:Component(parent), Light(CAMERA_TYPE::PERSPECTIVE, lightFlags)
	{
		m_pPointLight = new PointLight();
		m_pPointLight->position = { 0.0f,  2.0f,  0.0f, 0.0f };
		m_pPointLight->attenuation = { 1.0f, 0.09f, 0.032f, 0.0f };
		m_pPointLight->baseLight = *m_pBaseLight;
	}

	PointLightComponent::~PointLightComponent()
	{
		delete m_pPointLight;
	}

	void PointLightComponent::Update(double dt)
	{
		if (m_pCamera != nullptr)
		{
			m_pCamera->Update(dt);
		}

		if (m_LightFlags & FLAG_LIGHT::USE_TRANSFORM_POSITION)
		{
			Transform* tc = m_pParent->GetComponent<TransformComponent>()->GetTransform();
			float3 position = tc->GetPositionFloat3();
			m_pPointLight->position.x = position.x;
			m_pPointLight->position.y = position.y;
			m_pPointLight->position.z = position.z;
		}

		// Would use a global time counter if we had one, do not want to create a timer for each light
		static float timeTimer = 0;
		static float baseIntensity = 3.0f;
		if (m_LightFlags & FLAG_LIGHT::FLICKER)
		{
			timeTimer += dt * 2 * PI * m_flickerRate;
			float intensity = baseIntensity * flickerIntensityFunction(timeTimer);
			static float3 color = m_pBaseLight->color;
			float3 newColor = color * intensity;
			SetColor(newColor);
		}
	}

	void PointLightComponent::OnInitScene()
	{
		Renderer::GetInstance().InitPointLightComponent(this);
	}

	void PointLightComponent::OnUnInitScene()
	{
		Renderer::GetInstance().UnInitPointLightComponent(this);
	}

	void PointLightComponent::SetPosition(float3 position)
	{
		m_pPointLight->position = { position.x, position.y, position.z, 1.0f };
	}

	void PointLightComponent::SetAttenuation(float3 attenuation)
	{
		m_pPointLight->attenuation.x = attenuation.x;
		m_pPointLight->attenuation.y = attenuation.y;
		m_pPointLight->attenuation.z = attenuation.z;
	}

	void* PointLightComponent::GetLightData() const
	{
		return m_pPointLight;
	}

	void PointLightComponent::UpdateLightColorIntensity()
	{
		m_pPointLight->baseLight.color = m_pBaseLight->color;
		m_pPointLight->baseLight.intensity = m_pBaseLight->intensity;
	}
}