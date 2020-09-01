#ifndef MOUSEPICKER_H
#define MOUSEPICKER_H

class BaseCamera;
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

	bool IsPointInTriangle(DirectX::XMVECTOR& triV1, DirectX::XMVECTOR& triV2, DirectX::XMVECTOR& triV3, DirectX::XMVECTOR& point);
};

#endif
