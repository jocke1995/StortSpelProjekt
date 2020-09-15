#ifndef AUDIO3DLISTENERCOMPONENT_H
#define AUDIO3DLISTENERCOMPONENT_H

#include "Component.h"
#include <x3daudio.h>
//class AudioVoice;
class AudioEngine;

// Component used for setting the position and orientation of the "Listener" in 3D audio playback

namespace component
{
	class Audio3DListenerComponent : public Component
	{
	public:
		Audio3DListenerComponent(Entity* parent);
		virtual ~Audio3DListenerComponent();
		void Update(double dt);

		// update position and orientation of the listener
		void UpdatePosition();


	private:
		// 3D audio Listener struct, this contains world coordinates and orientation for the "listener" of 3D audio
		X3DAUDIO_LISTENER m_Listener;
	};
}

#endif