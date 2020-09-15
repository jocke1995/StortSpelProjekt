#ifndef AUDIO3DEMITTERCOMPONENT_H
#define AUDIO3DEMITTERCOMPONENT_H

#include "Component.h"
#include <x3daudio.h>
class AudioVoice;
class AudioEngine;

// Component used for setting/updating "Emitters" position and orientation in 3D audio playback

namespace component
{
	class Audio3DEmitterComponent : public Component
	{
	public:
		Audio3DEmitterComponent(Entity* parent);
		virtual ~Audio3DEmitterComponent();
		void Update(double dt);

		// update position and orientation of the emitter
		void UpdatePosition();

		// Clones an audiobuffer to create a voice to the component
		void AddVoice(const std::wstring& name);

		// Playback options of audio
		// Play audio
		void Play(const std::wstring& name);
		// Stop audio
		void Stop(const std::wstring& name);

	private:
		std::map<std::wstring, AudioVoice> m_Voices;
		// Emitter sets world positions of the audio source to be calculated for 3D sound
		X3DAUDIO_EMITTER m_Emitter;

	};
}

#endif