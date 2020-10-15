#include "stdafx.h"
#include "Light.h"

#include "../../Entity.h"

#include "../../../Renderer/Camera/OrthographicCamera.h"
#include "../../../Renderer/Camera/PerspectiveCamera.h"

Light::Light(CAMERA_TYPE camType, unsigned int lightFlags)
{
	m_Id = s_LightIdCounter++;

	m_LightFlags = lightFlags;

	m_pBaseLight = new BaseLight();
	m_pBaseLight->color= { 1.0f, 1.0f, 1.0f };
	m_pBaseLight->castShadow = false;
	m_CameraType = camType;
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

	UpdateLightIntensity();
}

unsigned int Light::GetLightFlags() const
{
	return m_LightFlags;
}

BaseCamera* Light::GetCamera() const
{
	return m_pCamera;
}

void Light::CreateCamera(float3 position, float3 direction)
{
	switch (m_CameraType)
	{
		case CAMERA_TYPE::ORTHOGRAPHIC:
			m_pCamera = new OrthographicCamera(
			{ position.x, position.y, position.z , 1.0f},
			{ direction.x, direction.y, direction.z , 0.0f});
			break; 
		case CAMERA_TYPE::PERSPECTIVE:
			m_pCamera = new PerspectiveCamera(
				{ position.x, position.y, position.z, 1.0f},
				{ direction.x, direction.y, direction.z , 0.0f},
				160.0f);	// Field of view
			break;
	}
}
