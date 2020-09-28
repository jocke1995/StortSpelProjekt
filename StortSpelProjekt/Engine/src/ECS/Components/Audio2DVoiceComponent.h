#ifndef AUDIOVOICECOMPONENT_H
#define AUDIOVOICECOMPONENT_H

#include "Component.h"
#include <Windows.h>

class AudioVoice;

// Component used for playing 2D/background sounds
namespace component
{
	class Audio2DVoiceComponent : public Component
	{
	public:
		Audio2DVoiceComponent(Entity* parent);
		virtual ~Audio2DVoiceComponent();

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
