#ifndef AUDIO3DLISTENERCOMPONENT_H
#define AUDIO3DLISTENERCOMPONENT_H

#include "Component.h"
#include <x3daudio.h>

class AudioEngine;
class Transform;

// Component used for setting the position and orientation of the "Listener" in 3D audio playback
namespace component
{
	class Audio3DListenerComponent : public Component
	{
	public:
		Audio3DListenerComponent(Entity* parent);
		virtual ~Audio3DListenerComponent();
		void Update(double dt);
		void InitScene();

		// updates position and orientation of the listener
		void UpdateListener();
		// set listener structure OrientFront, OrientTop, Position
		void SetListener(DirectX::XMFLOAT3 orientFront, DirectX::XMFLOAT3 orientTop, DirectX::XMFLOAT3 position);
		X3DAUDIO_LISTENER* GetListener();

	private:
		// 3D audio Listener struct, this contains world coordinates and orientation for the "listener" of 3D audio
		X3DAUDIO_LISTENER m_Listener;
		Transform* m_pTransform = nullptr;
	};
}

#endif
