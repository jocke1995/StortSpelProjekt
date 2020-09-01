#include "stdafx.h"
#include "DirectionalLightComponent.h"
#include "../Renderer/BaseCamera.h"

namespace component
{
	DirectionalLightComponent::DirectionalLightComponent(Entity* parent, unsigned int lightFlags)
		:Component(parent), Light(CAMERA_TYPE::ORTHOGRAPHIC, lightFlags)
	{
		this->directionalLight = new DirectionalLight();
		this->directionalLight->direction = { -1.0f,  -0.5f,  0.0f, 0.0f };
		this->directionalLight->baseLight = *this->m_pBaseLight;

		this->directionalLight->textureShadowMap = 0;

		this->initFlagUsages();
	}

	DirectionalLightComponent::~DirectionalLightComponent()
	{
		delete this->directionalLight;
	}


	void DirectionalLightComponent::Update(double dt)
	{
		if (this->m_pCamera != nullptr)
		{
			this->m_pCamera->Update(dt);
			this->directionalLight->viewProj = *this->m_pCamera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::SetDirection(float3 direction)
	{
		this->directionalLight->direction = { direction.x, direction.y, direction.z, 0.0f };
		
		if (this->m_pCamera != nullptr)
		{
			this->m_pCamera->SetPosition(-direction.x * 10, -direction.y * 10, -direction.z * 10);
			this->m_pCamera->SetLookAt(direction.x, direction.y, direction.z);
		}
	}

	void* DirectionalLightComponent::GetLightData() const
	{
		return this->directionalLight;
	}

	void DirectionalLightComponent::initFlagUsages()
	{
		if (this->m_LightFlags & FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION ||
			this->m_LightFlags & FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION ||
			this->m_LightFlags & FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION ||
			this->m_LightFlags & FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION)
		{
			this->CreateCamera(
				{
				-this->directionalLight->direction.x * 10,
				-this->directionalLight->direction.y * 10,
				-this->directionalLight->direction.z * 10 },
				{
				this->directionalLight->direction.x,
				this->directionalLight->direction.y,
				this->directionalLight->direction.z });

			this->directionalLight->baseLight.castShadow = true;

			this->directionalLight->viewProj = *this->m_pCamera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::UpdateLightData(COLOR_TYPE type)
	{
		switch (type)
		{
		case COLOR_TYPE::LIGHT_AMBIENT:
			this->directionalLight->baseLight.ambient = this->m_pBaseLight->ambient;
			break;
		case COLOR_TYPE::LIGHT_DIFFUSE:
			this->directionalLight->baseLight.diffuse = this->m_pBaseLight->diffuse;
			break;
		case COLOR_TYPE::LIGHT_SPECULAR:
			this->directionalLight->baseLight.specular = this->m_pBaseLight->specular;
			break;
		}
	}
}
