#include "stdafx.h"
#include "SpotLightComponent.h"
#include "EngineMath.h"

// Renderer
#include "../Renderer/Transform.h"
#include "../Renderer/BaseCamera.h"
#include "../Renderer/Renderer.h"

// ECS
#include "../ECS/Entity.h"

namespace component
{
    SpotLightComponent::SpotLightComponent(Entity* parent, unsigned int lightFlags)
        :Component(parent), Light(CAMERA_TYPE::PERSPECTIVE, lightFlags)
    {
        
        m_pSpotLight = new SpotLight();
        m_pSpotLight->position_cutOff = { 0.0f, 0.0f, 0.0f, cos(DirectX::XMConvertToRadians(30.0f)) };
        m_pSpotLight->direction_outerCutoff = { 1.0f, 0.0f, 0.0f, cos(DirectX::XMConvertToRadians(45.0f)) };
        m_pSpotLight->attenuation = { 1.0f, 0.009f, 0.0032f, 0.0f }; 
        m_pSpotLight->baseLight = *m_pBaseLight;

        m_pSpotLight->textureShadowMap = 0;

        initFlagUsages();
    }

    SpotLightComponent::~SpotLightComponent()
    {
        delete m_pSpotLight;
    }

    void SpotLightComponent::Update(double dt)
    {
        if (m_LightFlags & FLAG_LIGHT::USE_TRANSFORM_POSITION)
        {
            Transform* tc = m_pParent->GetComponent<TransformComponent>()->GetTransform();
            float3 position = tc->GetPositionFloat3();
            m_pSpotLight->position_cutOff.x = position.x;
            m_pSpotLight->position_cutOff.y = position.y;
            m_pSpotLight->position_cutOff.z = position.z;

            if (m_pCamera != nullptr)
            {
                m_pCamera->SetPosition(position.x, position.y, position.z);
            }
        }

        if (m_pCamera != nullptr)
        {
            m_pCamera->Update(dt);
            m_pSpotLight->viewProj = *m_pCamera->GetViewProjectionTranposed();
        }  
    }

    void SpotLightComponent::OnInitScene()
    {
        Renderer::GetInstance().InitSpotLightComponent(GetParent());
    }

    void SpotLightComponent::OnLoadScene()
    {
    }

	void SpotLightComponent::OnUnloadScene()
	{
	}

    void SpotLightComponent::SetPosition(float3 position)
    {
        m_pSpotLight->position_cutOff.x = position.x;
        m_pSpotLight->position_cutOff.y = position.y;
        m_pSpotLight->position_cutOff.z = position.z;

        if (m_pCamera != nullptr)
        {
            m_pCamera->SetPosition(position.x, position.y, position.z);
        }
    }

    void SpotLightComponent::SetCutOff(float cutOff)
    {
        m_pSpotLight->position_cutOff.w = cutOff;
    }

    void SpotLightComponent::SetDirection(float3 direction)
    {
        m_pSpotLight->direction_outerCutoff.x = direction.x;
        m_pSpotLight->direction_outerCutoff.y = direction.y;
        m_pSpotLight->direction_outerCutoff.z = direction.z;

        if (m_pCamera != nullptr)
        {
            m_pCamera->SetDirection(direction.x, direction.y, direction.z);
        }
    }

    void SpotLightComponent::SetOuterCutOff(float outerCutOff)
    {
        m_pSpotLight->direction_outerCutoff.w = outerCutOff;
    }

    void SpotLightComponent::SetAttenuation(float3 attenuation)
    {
        m_pSpotLight->attenuation.x = attenuation.x;
        m_pSpotLight->attenuation.y = attenuation.y;
        m_pSpotLight->attenuation.z = attenuation.z;
    }

    void* SpotLightComponent::GetLightData() const
    {
        return m_pSpotLight;
    }

    void SpotLightComponent::initFlagUsages()
    {
        if (m_LightFlags & FLAG_LIGHT::USE_TRANSFORM_POSITION)
        {
            Transform* tc = m_pParent->GetComponent<TransformComponent>()->GetTransform();
            float3 position = tc->GetPositionFloat3();
            m_pSpotLight->position_cutOff.x = position.x;
            m_pSpotLight->position_cutOff.y = position.y;
            m_pSpotLight->position_cutOff.z = position.z;
        }

        if (m_LightFlags & FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION ||
            m_LightFlags & FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION ||
            m_LightFlags & FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION ||
            m_LightFlags & FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION)
        {
            CreateCamera(
                {
                m_pSpotLight->position_cutOff.x,
                m_pSpotLight->position_cutOff.y,
                m_pSpotLight->position_cutOff.z,
                },
                {
                m_pSpotLight->direction_outerCutoff.x,
                m_pSpotLight->direction_outerCutoff.y,
                m_pSpotLight->direction_outerCutoff.z });

            m_pSpotLight->baseLight.castShadow = true;

            m_pSpotLight->viewProj = *m_pCamera->GetViewProjectionTranposed();
        }
    }

    void SpotLightComponent::UpdateLightIntensity()
    {
        m_pSpotLight->baseLight.color = m_pBaseLight->color;
    }
}