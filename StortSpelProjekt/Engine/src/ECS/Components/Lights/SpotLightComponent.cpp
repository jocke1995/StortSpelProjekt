#include "stdafx.h"
#include "SpotLightComponent.h"
#include "EngineMath.h"

// Renderer
#include "../Renderer/Transform.h"
#include "../Renderer/Camera/PerspectiveCamera.h"
#include "../Renderer/Renderer.h"
#include "../Misc/Option.h"

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
        this->Update(0);

        Renderer::GetInstance().InitSpotLightComponent(this);
    }

    void SpotLightComponent::OnUnInitScene()
    {
        Renderer::GetInstance().UnInitSpotLightComponent(this);
    }

    void SpotLightComponent::SetCutOff(float degrees)
    {
        m_pSpotLight->position_cutOff.w = cos(DirectX::XMConvertToRadians(degrees));
    }

    void SpotLightComponent::SetAttenuation(float3 attenuation)
    {
        m_pSpotLight->attenuation.x = attenuation.x;
        m_pSpotLight->attenuation.y = attenuation.y;
        m_pSpotLight->attenuation.z = attenuation.z;
    }

    // This function modifies the camera aswell as the position
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

    // This function modifies the camera aswell as the direction
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

    // This function modifies the camera aswell as the outerCutOff
    void SpotLightComponent::SetOuterCutOff(float degrees)
    {
        float cameraFov = degrees * 2.0f;
        if (degrees >= 89)
        {
            degrees = 89.0f;
            cameraFov = 179.0f;
        }

        m_pSpotLight->direction_outerCutoff.w = cos(DirectX::XMConvertToRadians(degrees));

        if (m_pCamera != nullptr)
        {
            PerspectiveCamera* persCam = static_cast<PerspectiveCamera*>(m_pCamera);
            persCam->SetFov(cameraFov);
        }
    }

    void SpotLightComponent::SetAspectRatio(float aspectRatio)
    {
        if (m_pCamera != nullptr)
        {
            PerspectiveCamera* persCam = static_cast<PerspectiveCamera*>(m_pCamera);
            persCam->SetAspectRatio(aspectRatio);
        }
    }

    void SpotLightComponent::SetNearPlaneDistance(float nearZ)
    {
        if (m_pCamera != nullptr)
        {
            PerspectiveCamera* persCam = static_cast<PerspectiveCamera*>(m_pCamera);
            persCam->SetNearZ(nearZ);
        }
    }

    void SpotLightComponent::SetFarPlaneDistance(float farZ)
    {
        if (m_pCamera != nullptr)
        {
            PerspectiveCamera* persCam = static_cast<PerspectiveCamera*>(m_pCamera);
            persCam->SetFarZ(farZ);
        }
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

        if (m_LightFlags & FLAG_LIGHT::CAST_SHADOW)
        {
            int textFileSetting = std::stoi(Option::GetInstance().GetVariable("i_shadowResolution").c_str());
            if (textFileSetting >= 0)
            {
                CreatePerspectiveCamera(
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
    }

    void SpotLightComponent::UpdateLightColor()
    {
        m_pSpotLight->baseLight.color = m_pBaseLight->color;
    }
}
