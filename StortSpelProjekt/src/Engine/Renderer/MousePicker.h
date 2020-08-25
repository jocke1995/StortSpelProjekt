#ifndef MOUSEPICKER_H
#define MOUSEPICKER_H

#include "BaseCamera.h"
#include "../ECS/Components/BoundingBoxComponent.h"

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
	bool Pick(component::BoundingBoxComponent* bbc, float& distance);

private:
	BaseCamera* primaryCamera = nullptr;

	DirectX::XMVECTOR rayInWorldSpacePos = {};
	DirectX::XMVECTOR rayInWorldSpaceDir = {};

	bool IsPointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point);
};

#endif
