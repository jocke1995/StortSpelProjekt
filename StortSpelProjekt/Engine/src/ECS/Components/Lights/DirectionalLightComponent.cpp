#include "stdafx.h"
#include "DirectionalLightComponent.h"
#include "../Renderer/Camera/BaseCamera.h"
#include "../Renderer/Renderer.h"
#include "../Misc/Option.h"

namespace component
{
	DirectionalLightComponent::DirectionalLightComponent(Entity* parent, unsigned int lightFlags)
		:Component(parent), Light(CAMERA_TYPE::ORTHOGRAPHIC, lightFlags)
	{
		m_pDirectionalLight = new DirectionalLight();
		m_pDirectionalLight->direction = { -1.0f,  -0.5f,  0.0f, 0.0f };
		m_pDirectionalLight->baseLight = *m_pBaseLight;

		m_pDirectionalLight->textureShadowMap = 0;

		initFlagUsages();
	}

	DirectionalLightComponent::~DirectionalLightComponent()
	{
		delete m_pDirectionalLight;
	}


	void DirectionalLightComponent::Update(double dt)
	{
		if (m_pCamera != nullptr)
		{
			m_pCamera->Update(dt);
			m_pDirectionalLight->viewProj = *m_pCamera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::OnInitScene()
	{
		this->Update(0);

		Renderer::GetInstance().InitDirectionalLightComponent(GetParent());
	}

	void DirectionalLightComponent::OnLoadScene()
	{
	}

	void DirectionalLightComponent::OnUnloadScene()
	{
	}

	void DirectionalLightComponent::SetDirection(float3 direction)
	{
		m_pDirectionalLight->direction = { direction.x, direction.y, direction.z, 0.0f };
		
		if (m_pCamera != nullptr)
		{
			m_pCamera->SetPosition(-direction.x * 30, -direction.y * 30, -direction.z * 30);
			m_pCamera->SetDirection(direction.x, direction.y, direction.z);
		}
	}

	void* DirectionalLightComponent::GetLightData() const
	{
		return m_pDirectionalLight;
	}

	void DirectionalLightComponent::initFlagUsages()
	{
		if (m_LightFlags & FLAG_LIGHT::CAST_SHADOW)
		{
			CreateCamera(
				{
				-m_pDirectionalLight->direction.x * 10,
				-m_pDirectionalLight->direction.y * 10,
				-m_pDirectionalLight->direction.z * 10 },
				{
				m_pDirectionalLight->direction.x,
				m_pDirectionalLight->direction.y,
				m_pDirectionalLight->direction.z });

			if (std::stoi(Option::GetInstance().GetVariable("i_shadowResolution").c_str()) >= 0)
			{
				m_pDirectionalLight->baseLight.castShadow = true;
			}

			m_pDirectionalLight->viewProj = *m_pCamera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::UpdateLightIntensity()
	{
		m_pDirectionalLight->baseLight.color = m_pBaseLight->color;
	}
}
