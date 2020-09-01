#ifndef LIGHT_H
#define LIGHT_H
#include "Core.h"
#include "structs.h"

class BaseCamera;
enum FLAG_LIGHT
{
	// Set flag to make the light position inherit the position of the corresponding m_pMesh
	USE_TRANSFORM_POSITION = BIT(1),

	// Option to make the light cast shadows or not with different resolutions
	CAST_SHADOW_LOW_RESOLUTION = BIT(2),
	CAST_SHADOW_MEDIUM_RESOLUTION = BIT(3),
	CAST_SHADOW_HIGH_RESOLUTION = BIT(4),
	CAST_SHADOW_ULTRA_RESOLUTION = BIT(5),

	// If this is set, m_pRenderer only need to copy data once to GPU
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
	BaseLight* m_pBaseLight = nullptr;
	unsigned int m_LightFlags = 0;

	BaseCamera* m_pCamera = nullptr;
	CAMERA_TYPE m_CameraType;
	void CreateCamera(float3 position, float3 lookAt);

	virtual void UpdateLightData(COLOR_TYPE type) = 0;

};

#endif