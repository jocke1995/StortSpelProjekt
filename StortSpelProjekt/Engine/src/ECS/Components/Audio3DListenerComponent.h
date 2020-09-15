#ifndef AUDIO3DLISTENERCOMPONENT_H
#define AUDIO3DLISTENERCOMPONENT_H

#include "Component.h"
//class AudioVoice;
//class AudioEngine;

// Component used for playing sounds

namespace component
{
	class Audio3DListenerComponent : public Component
	{
	public:
		Audio3DListenerComponent(Entity* parent);
		virtual ~Audio3DListenerComponent();
		void Update(double dt);



	private:

	};
}

#endif