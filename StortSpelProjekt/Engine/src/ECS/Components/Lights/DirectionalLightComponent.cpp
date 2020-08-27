#include "DirectionalLightComponent.h"

namespace component
{
	DirectionalLightComponent::DirectionalLightComponent(Entity* parent, unsigned int lightFlags)
		:Component(parent), Light(CAMERA_TYPE::ORTHOGRAPHIC, lightFlags)
	{
		this->directionalLight = new DirectionalLight();
		this->directionalLight->direction = { -1.0f,  -0.5f,  0.0f, 0.0f };
		this->directionalLight->baseLight = *this->baseLight;

		this->directionalLight->textureShadowMap = 0;

		this->InitFlagUsages();
	}

	DirectionalLightComponent::~DirectionalLightComponent()
	{
		delete this->directionalLight;
	}


	void DirectionalLightComponent::Update(double dt)
	{
		if (this->camera != nullptr)
		{
			this->camera->Update(dt);
			this->directionalLight->viewProj = *this->camera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::SetDirection(float3 direction)
	{
		this->directionalLight->direction = { direction.x, direction.y, direction.z, 0.0f };
		
		if (this->camera != nullptr)
		{
			this->camera->SetPosition(-direction.x * 10, -direction.y * 10, -direction.z * 10);
			this->camera->SetLookAt(direction.x, direction.y, direction.z);
		}
	}

	void* DirectionalLightComponent::GetLightData() const
	{
		return this->directionalLight;
	}

	void DirectionalLightComponent::InitFlagUsages()
	{
		if (this->lightFlags & FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION ||
			this->lightFlags & FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION ||
			this->lightFlags & FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION ||
			this->lightFlags & FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION)
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

			this->directionalLight->viewProj = *this->camera->GetViewProjectionTranposed();
		}
	}

	void DirectionalLightComponent::UpdateLightData(LIGHT_COLOR_TYPE type)
	{
		switch (type)
		{
		case LIGHT_COLOR_TYPE::LIGHT_AMBIENT:
			this->directionalLight->baseLight.ambient = this->baseLight->ambient;
			break;
		case LIGHT_COLOR_TYPE::LIGHT_DIFFUSE:
			this->directionalLight->baseLight.diffuse = this->baseLight->diffuse;
			break;
		case LIGHT_COLOR_TYPE::LIGHT_SPECULAR:
			this->directionalLight->baseLight.specular = this->baseLight->specular;
			break;
		}
	}
}
