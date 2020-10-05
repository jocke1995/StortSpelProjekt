#ifndef MOUSEPICKER_H
#define MOUSEPICKER_H

#include "../ECS/Components/BoundingBoxComponent.h"
#include "EngineMath.h"
class BaseCamera;


// Todo: Currently picking from middle of the screen (like a first person shooter)
// Add real mouse coordinates when event-system + inputHandler is implemented
class MousePicker
{
public:
	MousePicker();
	virtual ~MousePicker();

	void SetPrimaryCamera(BaseCamera* primaryCamera);

	void UpdateRay();

	// returns true if the BoundingBox intersected with ray
	bool Pick(component::BoundingBoxComponent* bbc, float& distance, unsigned int bbdIndex);

private:
	BaseCamera* m_pPrimaryCamera = nullptr;
	DirectX::XMVECTOR m_RayInWorldSpacePos = {};
	DirectX::XMVECTOR m_RayInWorldSpaceDir = {};

	bool isPointInTriangle(DirectX::XMVECTOR& triV1, DirectX::XMVECTOR& triV2, DirectX::XMVECTOR& triV3, DirectX::XMVECTOR& point);
};

#endif
