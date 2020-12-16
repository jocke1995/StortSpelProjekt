#include "stdafx.h"
#include "Light.h"

#include "../../Entity.h"
#include "../../Engine/src/Misc/EngineRand.h"

#include "../../../Renderer/Camera/OrthographicCamera.h"
#include "../../../Renderer/Camera/PerspectiveCamera.h"

Light::Light(CAMERA_TYPE camType, unsigned int lightFlags)
{
	m_Id = s_LightIdCounter++;

	m_LightFlags = lightFlags;

	m_pBaseLight = new BaseLight();
	m_pBaseLight->color= { 1.0f, 1.0f, 1.0f };
	m_pBaseLight->intensity = 1.0f;
	m_UnflickeredIntensity = m_pBaseLight->intensity;
	m_pBaseLight->castShadow = false;
	m_CameraType = camType;


	// Give each light a different time offset
	static EngineRand er = EngineRand(5);
	m_FlickerTimer = er.Randf(1, 20000) / 20000;
}

Light::~Light()
{
	delete m_pBaseLight;

	delete m_pCamera;
}

bool Light::operator==(const Light& other)
{
	return m_Id == other.m_Id;
}

void Light::SetColor(float3 color)
{
	m_pBaseLight->color = color;
	m_pBaseLight->color.normalize();
	UpdateLightColorIntensity();
}

void Light::SetIntensity(float intensity)
{
	m_pBaseLight->intensity = intensity;
	m_UnflickeredIntensity = intensity;
	UpdateLightColorIntensity();
}

void Light::SetFlickerRate(float rate)
{
	m_FlickerRate = rate;
}

void Light::SetFlickerAmplitude(float amplitude)
{
	m_FlickerAmplitude = amplitude;
}

unsigned int Light::GetLightFlags() const
{
	return m_LightFlags;
}

BaseCamera* Light::GetCamera() const
{
	return m_pCamera;
}

void Light::CreateOrthographicCamera(
	float3 position, float3 direction,
	float left,
	float right,
	float bot,
	float top,
	float nearZ,
	float farZ)
{
	DirectX::XMVECTOR pos = { position.x, position.y, position.z , 1.0f };
	DirectX::XMVECTOR dir = { direction.x, direction.y, direction.z , 0.0f };

		m_pCamera = new OrthographicCamera(
			pos, dir,
			left, right,
			bot, top,
			nearZ, farZ);
}

void Light::CreatePerspectiveCamera(
	float3 position, float3 direction,
	float fov,
	float aspectRatio,
	float nearZ, float farZ)
{
	DirectX::XMVECTOR pos = { position.x, position.y, position.z , 1.0f };
	DirectX::XMVECTOR dir = { direction.x, direction.y, direction.z , 0.0f };

	m_pCamera = new PerspectiveCamera(
		pos, dir,
		fov,
		aspectRatio,
		nearZ,
		farZ);
}

void Light::flicker(double dt)
{
	if (m_LightFlags & FLAG_LIGHT::FLICKER)
	{
		m_FlickerTimer += dt * 2 * PI * m_FlickerRate;
		float intensity = m_UnflickeredIntensity * flickerIntensityFunction(m_FlickerTimer);
		if (intensity < 0)
		{
			intensity = 0;
		}

		// Set intensity without setting unflickeredintensity
		m_pBaseLight->intensity = intensity;
		UpdateLightColorIntensity();
	}
}
