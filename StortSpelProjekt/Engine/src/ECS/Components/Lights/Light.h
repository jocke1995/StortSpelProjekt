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
	CAST_SHADOW = BIT(2),

	// If this is set, m_pRenderer only need to copy data once to GPU
	// STATIC_DATA .. = BIT(3),

	// etc..
	NUM_FLAGS_LIGHT = 2
};

static unsigned int s_LightIdCounter = 0;

class Light
{
public:
	Light(CAMERA_TYPE camType, unsigned int lightFlags = 0);
	virtual ~Light();

	bool operator== (const Light& other);

	virtual void Update(double dt) = 0;

	void SetColor(float3 color);

	// Gets
	unsigned int GetLightFlags() const;
	virtual void* GetLightData() const = 0;
	BaseCamera* GetCamera() const;

protected:
	BaseLight* m_pBaseLight = nullptr;
	unsigned int m_LightFlags = 0;
	unsigned int m_Id = 0;

	BaseCamera* m_pCamera = nullptr;
	CAMERA_TYPE m_CameraType;
	void CreateCamera(float3 position, float3 direction);

	virtual void UpdateLightIntensity() = 0;

};

#endif