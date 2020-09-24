#include "stdafx.h"
#include "DirectionalLightComponent.h"
#include "../Renderer/BaseCamera.h"
#include "../Renderer/Renderer.h"

namespace component
{
	DirectionalLightComponent::DirectionalLightComponent(Entity* parent, unsigned int lightFlags)
		:Component(parent), Light(CAMERA_TYPE::ORTHOGRAPHIC, lightFlags)
	{
		directionalLight = new DirectionalLight();
		directionalLight->direction = { -1.0f,  -0.5f,  0.0f, 0.0f };
		directionalLight->baseLight = *m_pBaseLight;

		directionalLight->textureShadowMap = 0;

		initFlagUsages();
	}

	DirectionalLightComponent::~DirectionalLightComponent()
	{
		delete directionalLight;
	}


	void DirectionalLightComponent::Update(double dt)
	{
		if (m_pCamera != nullptr)
		{
			m_pCamera->Update(dt);
			directionalLight->viewProj = *m_pCamera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::InitScene(Renderer* renderer)
	{
		Log::Print("DirectionalLightComponent Init called!\n");
		renderer->InitDirectionalLightComponent(GetParent());
	}

	void DirectionalLightComponent::SetDirection(float3 direction)
	{
		directionalLight->direction = { direction.x, direction.y, direction.z, 0.0f };
		
		if (m_pCamera != nullptr)
		{
			m_pCamera->SetPosition(-direction.x * 10, -direction.y * 10, -direction.z * 10);
			m_pCamera->SetDirection(direction.x, direction.y, direction.z);
		}
	}

	void* DirectionalLightComponent::GetLightData() const
	{
		return directionalLight;
	}

	void DirectionalLightComponent::initFlagUsages()
	{
		if (m_LightFlags & FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION ||
			m_LightFlags & FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION ||
			m_LightFlags & FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION ||
			m_LightFlags & FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION)
		{
			CreateCamera(
				{
				-directionalLight->direction.x * 10,
				-directionalLight->direction.y * 10,
				-directionalLight->direction.z * 10 },
				{
				directionalLight->direction.x,
				directionalLight->direction.y,
				directionalLight->direction.z });

			directionalLight->baseLight.castShadow = true;

			directionalLight->viewProj = *m_pCamera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::UpdateLightData(COLOR_TYPE type)
	{
		switch (type)
		{
		case COLOR_TYPE::LIGHT_AMBIENT:
			directionalLight->baseLight.ambient = m_pBaseLight->ambient;
			break;
		case COLOR_TYPE::LIGHT_DIFFUSE:
			directionalLight->baseLight.diffuse = m_pBaseLight->diffuse;
			break;
		case COLOR_TYPE::LIGHT_SPECULAR:
			directionalLight->baseLight.specular = m_pBaseLight->specular;
			break;
		}
	}
}
