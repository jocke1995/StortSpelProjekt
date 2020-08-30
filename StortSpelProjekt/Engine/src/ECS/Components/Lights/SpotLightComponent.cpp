#include "SpotLightComponent.h"

namespace component
{
    SpotLightComponent::SpotLightComponent(Entity* parent, unsigned int lightFlags)
        :Component(parent), Light(CAMERA_TYPE::PERSPECTIVE, lightFlags)
    {
        
        this->spotLight = new SpotLight();
        this->spotLight->position_cutOff = { 0.0f, 0.0f, 0.0f, cos(XMConvertToRadians(30.0f)) };
        this->spotLight->direction_outerCutoff = { 1.0f, 0.0f, 0.0f, cos(XMConvertToRadians(45.0f)) };
        this->spotLight->attenuation = { 1.0f, 0.009f, 0.0032f, 0.0f }; 
        this->spotLight->baseLight = *this->baseLight;

        this->spotLight->textureShadowMap = 0;

        this->InitFlagUsages();
    }

    SpotLightComponent::~SpotLightComponent()
    {
        delete this->spotLight;
    }

    void SpotLightComponent::Update(double dt)
    {
        if (this->lightFlags & FLAG_LIGHT::USE_TRANSFORM_POSITION)
        {
            Transform* tc = this->parent->GetComponent<TransformComponent>()->GetTransform();
            float3 position = tc->GetPositionFloat3();
            this->spotLight->position_cutOff.x = position.x;
            this->spotLight->position_cutOff.y = position.y;
            this->spotLight->position_cutOff.z = position.z;

            if (this->camera != nullptr)
            {
                this->camera->SetPosition(position.x, position.y, position.z);
            }
        }

        if (this->camera != nullptr)
        {
            this->camera->Update(dt);
            this->spotLight->viewProj = *this->camera->GetViewProjectionTranposed();
        }  
    }

    void SpotLightComponent::SetPosition(float3 position)
    {
        this->spotLight->position_cutOff.x = position.x;
        this->spotLight->position_cutOff.y = position.y;
        this->spotLight->position_cutOff.z = position.z;

        if (this->camera != nullptr)
        {
            this->camera->SetPosition(position.x, position.y, position.z);
        }
    }

    void SpotLightComponent::SetCutOff(float cutOff)
    {
        this->spotLight->position_cutOff.w = cutOff;
    }

    void SpotLightComponent::SetDirection(float3 direction)
    {
        this->spotLight->direction_outerCutoff.x = direction.x;
        this->spotLight->direction_outerCutoff.y = direction.y;
        this->spotLight->direction_outerCutoff.z = direction.z;

        if (this->camera != nullptr)
        {
            this->camera->SetLookAt(direction.x, direction.y, direction.z);
        }
    }

    void SpotLightComponent::SetOuterCutOff(float outerCutOff)
    {
        this->spotLight->direction_outerCutoff.w = outerCutOff;
    }

    void SpotLightComponent::SetAttenuation(float3 attenuation)
    {
        this->spotLight->attenuation.x = attenuation.x;
        this->spotLight->attenuation.y = attenuation.y;
        this->spotLight->attenuation.z = attenuation.z;
    }

    void* SpotLightComponent::GetLightData() const
    {
        return this->spotLight;
    }

    void SpotLightComponent::InitFlagUsages()
    {
        if (this->lightFlags & FLAG_LIGHT::USE_TRANSFORM_POSITION)
        {
            Transform* tc = this->parent->GetComponent<TransformComponent>()->GetTransform();
            float3 position = tc->GetPositionFloat3();
            this->spotLight->position_cutOff.x = position.x;
            this->spotLight->position_cutOff.y = position.y;
            this->spotLight->position_cutOff.z = position.z;
        }

        if (this->lightFlags & FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION ||
            this->lightFlags & FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION ||
            this->lightFlags & FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION ||
            this->lightFlags & FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION)
        {
            this->CreateCamera(
                {
                this->spotLight->position_cutOff.x,
                this->spotLight->position_cutOff.y,
                this->spotLight->position_cutOff.z,
                },
                {
                this->spotLight->direction_outerCutoff.x,
                this->spotLight->direction_outerCutoff.y,
                this->spotLight->direction_outerCutoff.z });

            this->spotLight->baseLight.castShadow = true;

            this->spotLight->viewProj = *this->camera->GetViewProjectionTranposed();
        }
    }

    void SpotLightComponent::UpdateLightData(COLOR_TYPE type)
    {
        switch (type)
        {
        case COLOR_TYPE::LIGHT_AMBIENT:
            this->spotLight->baseLight.ambient = this->baseLight->ambient;
            break;
        case COLOR_TYPE::LIGHT_DIFFUSE:
            this->spotLight->baseLight.diffuse = this->baseLight->diffuse;
            break;
        case COLOR_TYPE::LIGHT_SPECULAR:
            this->spotLight->baseLight.specular = this->baseLight->specular;
            break;
        }
    }
}