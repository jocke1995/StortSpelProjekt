#include "stdafx.h"
#include "Light.h"

#include "../../Entity.h"

#include "../../../Renderer/OrthographicCamera.h"
#include "../../../Renderer/PerspectiveCamera.h"

Light::Light(CAMERA_TYPE camType, unsigned int lightFlags)
{
	this->cameraType = camType;

	this->lightFlags = lightFlags;

	this->baseLight = new BaseLight();
	this->baseLight->ambient = { 0.05f, 0.05f, 0.05f, 1.0f };
	this->baseLight->diffuse = { 0.35f, 0.35f, 0.35f, 1.0f };
	this->baseLight->specular = { 0.2f, 0.2f, 0.2f, 1.0f };
	this->baseLight->castShadow = false;
}

Light::~Light()
{
	delete this->baseLight;

	delete this->camera;
}

void Light::SetColor(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		this->baseLight->ambient = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		this->baseLight->diffuse = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		this->baseLight->specular = color;
		break;
	}

	this->UpdateLightData(type);
}

unsigned int Light::GetLightFlags() const
{
	return this->lightFlags;
}

BaseCamera* Light::GetCamera() const
{
	return this->camera;
}

void Light::CreateCamera(float3 position, float3 lookAt)
{
	switch (this->cameraType)
	{
		case CAMERA_TYPE::ORTHOGRAPHIC:
			this->camera = new OrthographicCamera(
			{ position.x, position.y, position.z , 1.0f},
			{ lookAt.x, lookAt.y, lookAt.z , 0.0f});
			break; 
		case CAMERA_TYPE::PERSPECTIVE:
			this->camera = new PerspectiveCamera(
				{ position.x, position.y, position.z, 1.0f},
				{ lookAt.x, lookAt.y, lookAt.z , 0.0f},
				60.0f);	// Field of view
			break;
	}
}
