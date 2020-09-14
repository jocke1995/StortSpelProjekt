#ifndef AUDIOVOICECOMPONENT_H
#define AUDIOVOICECOMPONENT_H

#include "Component.h"
class AudioVoice;
class AudioEngine;

// Component used for playing sounds

namespace component
{
	class AudioVoiceComponent : public Component
	{
	public:
		AudioVoiceComponent(Entity* parent);
		virtual ~AudioVoiceComponent();
		void Update(double dt);

		// Clones an audiobuffer to create a voice to the component
		void AddVoice(const std::wstring& name);

		// Playback options of audio
		// Play audio
		void Play(const std::wstring& name);
		// Stop audio
		void Stop(const std::wstring& name);

	private:
		std::map<std::wstring, AudioVoice> m_Voices;
	};
}

#endif