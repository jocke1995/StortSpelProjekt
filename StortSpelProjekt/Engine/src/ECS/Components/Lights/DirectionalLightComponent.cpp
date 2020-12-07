#include "stdafx.h"
#include "DirectionalLightComponent.h"
#include "../Renderer/Camera/OrthographicCamera.h"
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
		Renderer::GetInstance().InitDirectionalLightComponent(this);
	}

	void DirectionalLightComponent::OnUnInitScene()
	{
		Renderer::GetInstance().UnInitDirectionalLightComponent(this);
	}

	void DirectionalLightComponent::SetCameraDistance(float distance)
	{
		m_Distance = distance;
	}

	void DirectionalLightComponent::SetDirection(float3 direction)
	{
		m_pDirectionalLight->direction = { direction.x, direction.y, direction.z, 0.0f };
		
		if (m_pCamera != nullptr)
		{
			m_pCamera->SetPosition(-direction.x * m_Distance, -direction.y * m_Distance, -direction.z * m_Distance);
			m_pCamera->SetDirection(direction.x, direction.y, direction.z);
		}
	}

	void DirectionalLightComponent::SetCameraLeft(float left)
	{
		OrthographicCamera* ogCamera = static_cast<OrthographicCamera*>(m_pCamera);
		ogCamera->SetLeft(left);
	}

	void DirectionalLightComponent::SetCameraRight(float right)
	{
		OrthographicCamera* ogCamera = static_cast<OrthographicCamera*>(m_pCamera);
		ogCamera->SetRight(right);
	}

	void DirectionalLightComponent::SetCameraBot(float bot)
	{
		OrthographicCamera* ogCamera = static_cast<OrthographicCamera*>(m_pCamera);
		ogCamera->SetBot(bot);
	}

	void DirectionalLightComponent::SetCameraTop(float top)
	{
		OrthographicCamera* ogCamera = static_cast<OrthographicCamera*>(m_pCamera);
		ogCamera->SetTop(top);
	}

	void DirectionalLightComponent::SetCameraNearZ(float nearPlaneDistance)
	{
		OrthographicCamera* ogCamera = static_cast<OrthographicCamera*>(m_pCamera);
		ogCamera->SetNearZ(nearPlaneDistance);
	}

	void DirectionalLightComponent::SetCameraFarZ(float farPlaneDistance)
	{
		OrthographicCamera* ogCamera = static_cast<OrthographicCamera*>(m_pCamera);
		ogCamera->SetFarZ(farPlaneDistance);
	}

	void* DirectionalLightComponent::GetLightData() const
	{
		return m_pDirectionalLight;
	}

	void DirectionalLightComponent::UpdateLightColor()
	{
		m_pDirectionalLight->baseLight.color = m_pBaseLight->color;
	}

	void DirectionalLightComponent::initFlagUsages()
	{
		if (m_LightFlags & FLAG_LIGHT::CAST_SHADOW)
		{
			int textFileSetting = std::stoi(Option::GetInstance().GetVariable("i_shadowResolution").c_str());
			if (textFileSetting >= 0)
			{
				CreateOrthographicCamera(
					{
					-m_pDirectionalLight->direction.x * 10,
					-m_pDirectionalLight->direction.y * 10,
					-m_pDirectionalLight->direction.z * 10 },
					{
					m_pDirectionalLight->direction.x,
					m_pDirectionalLight->direction.y,
					m_pDirectionalLight->direction.z });

			
				m_pDirectionalLight->baseLight.castShadow = true;

				m_pDirectionalLight->viewProj = *m_pCamera->GetViewProjectionTranposed();
			}
		}
	}
}
