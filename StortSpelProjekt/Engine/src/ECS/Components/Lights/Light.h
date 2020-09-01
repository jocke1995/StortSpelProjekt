#ifndef LIGHT_H
#define LIGHT_H

#include "../../Entity.h"

#include "../../../Renderer/OrthographicCamera.h"
#include "../../../Renderer/PerspectiveCamera.h"
#include "structs.h"

enum FLAG_LIGHT
{
	// Set flag to make the light position inherit the position of the corresponding mesh
	USE_TRANSFORM_POSITION = BIT(1),

	// Option to make the light cast shadows or not with different resolutions
	CAST_SHADOW_LOW_RESOLUTION = BIT(2),
	CAST_SHADOW_MEDIUM_RESOLUTION = BIT(3),
	CAST_SHADOW_HIGH_RESOLUTION = BIT(4),
	CAST_SHADOW_ULTRA_RESOLUTION = BIT(5),

	// If this is set, renderer only need to copy data once to GPU
	// STATIC_DATA .. = BIT(6),

	// etc..
};

class Light
{
public:
	Light(CAMERA_TYPE camType, unsigned int lightFlags = 0);
	virtual ~Light();

	virtual void Update(double dt) = 0;

	void SetColor(COLOR_TYPE type, float4 color);

	// Gets
	unsigned int GetLightFlags() const;
	virtual void* GetLightData() const = 0;
	BaseCamera* GetCamera() const;

protected:
	BaseLight* baseLight = nullptr;
	virtual void UpdateLightData(COLOR_TYPE type) = 0;

	unsigned int lightFlags = 0;

	BaseCamera* camera = nullptr;
	CAMERA_TYPE cameraType;
	void CreateCamera(float3 position, float3 lookAt);

};

#endif