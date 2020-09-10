#include "stdafx.h"
#include "PointLightComponent.h"

// Renderer
#include "../Renderer/Transform.h"
#include "../Renderer/BaseCamera.h"

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
			m_pCamera->Update(m_pParent, dt);
		}

		if (m_LightFlags & FLAG_LIGHT::USE_TRANSFORM_POSITION)
		{
			Transform* tc = m_pParent->GetComponent<TransformComponent>()->GetTransform();
			float3 position = tc->GetPositionFloat3();
			m_pPointLight->position.x = position.x;
			m_pPointLight->position.y = position.y;
			m_pPointLight->position.z = position.z;
		}
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

	void PointLightComponent::UpdateLightData(COLOR_TYPE type)
	{
		switch (type)
		{
		case COLOR_TYPE::LIGHT_AMBIENT:
			m_pPointLight->baseLight.ambient = m_pBaseLight->ambient;
			break;
		case COLOR_TYPE::LIGHT_DIFFUSE:
			m_pPointLight->baseLight.diffuse = m_pBaseLight->diffuse;
			break;
		case COLOR_TYPE::LIGHT_SPECULAR:
			m_pPointLight->baseLight.specular = m_pBaseLight->specular;
			break;
		}
	}
}