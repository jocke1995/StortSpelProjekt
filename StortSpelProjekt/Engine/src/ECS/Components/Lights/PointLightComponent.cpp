#include "PointLightComponent.h"

namespace component
{
	PointLightComponent::PointLightComponent(Entity* parent, unsigned int lightFlags)
		:Component(parent), Light(CAMERA_TYPE::PERSPECTIVE, lightFlags)
	{
		this->pointLight = new PointLight();
		this->pointLight->position = { 0.0f,  2.0f,  0.0f, 0.0f };
		this->pointLight->attenuation = { 1.0f, 0.09f, 0.032f, 0.0f };
		this->pointLight->baseLight = *this->baseLight;
	}

	PointLightComponent::~PointLightComponent()
	{
		delete this->pointLight;
	}

	void PointLightComponent::Update(double dt)
	{
		if (this->camera != nullptr)
		{
			this->camera->Update(dt);
		}

		if (this->lightFlags & FLAG_LIGHT::USE_TRANSFORM_POSITION)
		{
			Transform* tc = this->parent->GetComponent<TransformComponent>()->GetTransform();
			float3 position = tc->GetPositionFloat3();
			this->pointLight->position.x = position.x;
			this->pointLight->position.y = position.y;
			this->pointLight->position.z = position.z;
		}
	}

	void PointLightComponent::SetPosition(float3 position)
	{
		this->pointLight->position = { position.x, position.y, position.z, 1.0f };
	}

	void PointLightComponent::SetAttenuation(float3 attenuation)
	{
		this->pointLight->attenuation.x = attenuation.x;
		this->pointLight->attenuation.y = attenuation.y;
		this->pointLight->attenuation.z = attenuation.z;
	}

	void* PointLightComponent::GetLightData() const
	{
		return this->pointLight;
	}

	void PointLightComponent::UpdateLightData(COLOR_TYPE type)
	{
		switch (type)
		{
		case COLOR_TYPE::LIGHT_AMBIENT:
			this->pointLight->baseLight.ambient = this->baseLight->ambient;
			break;
		case COLOR_TYPE::LIGHT_DIFFUSE:
			this->pointLight->baseLight.diffuse = this->baseLight->diffuse;
			break;
		case COLOR_TYPE::LIGHT_SPECULAR:
			this->pointLight->baseLight.specular = this->baseLight->specular;
			break;
		}
	}
}