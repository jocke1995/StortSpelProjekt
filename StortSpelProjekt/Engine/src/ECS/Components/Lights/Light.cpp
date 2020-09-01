#include "stdafx.h"
#include "Light.h"

#include "../../Entity.h"

#include "../../../Renderer/OrthographicCamera.h"
#include "../../../Renderer/PerspectiveCamera.h"

Light::Light(CAMERA_TYPE camType, unsigned int lightFlags)
{
	//#include "stdafx.h"m_CameraType = m_CamType;

	m_LightFlags = lightFlags;

	m_pBaseLight = new BaseLight();
	m_pBaseLight->ambient = { 0.05f, 0.05f, 0.05f, 1.0f };
	m_pBaseLight->diffuse = { 0.35f, 0.35f, 0.35f, 1.0f };
	m_pBaseLight->specular = { 0.2f, 0.2f, 0.2f, 1.0f };
	m_pBaseLight->castShadow = false;
	m_CameraType = camType;
}

Light::~Light()
{
	delete m_pBaseLight;

	delete m_pCamera;
}

void Light::SetColor(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		m_pBaseLight->ambient = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		m_pBaseLight->diffuse = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		m_pBaseLight->specular = color;
		break;
	}

	UpdateLightData(type);
}

unsigned int Light::GetLightFlags() const
{
	return m_LightFlags;
}

BaseCamera* Light::GetCamera() const
{
	return m_pCamera;
}

void Light::CreateCamera(float3 position, float3 lookAt)
{
	switch (m_CameraType)
	{
		case CAMERA_TYPE::ORTHOGRAPHIC:
			m_pCamera = new OrthographicCamera(
			{ position.x, position.y, position.z , 1.0f},
			{ lookAt.x, lookAt.y, lookAt.z , 0.0f});
			break; 
		case CAMERA_TYPE::PERSPECTIVE:
			m_pCamera = new PerspectiveCamera(
				{ position.x, position.y, position.z, 1.0f},
				{ lookAt.x, lookAt.y, lookAt.z , 0.0f},
				60.0f);	// Field of view
			break;
	}
}
