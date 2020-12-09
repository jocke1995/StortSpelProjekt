#ifndef AUDIOVOICECOMPONENT_H
#define AUDIOVOICECOMPONENT_H

#include "Component.h"
#include <Windows.h>

class AudioVoice;
struct PauseGame;

// Component used for playing 2D/background sounds
namespace component
{
	class Audio2DVoiceComponent : public Component
	{
	public:
		Audio2DVoiceComponent(Entity* parent);
		virtual ~Audio2DVoiceComponent();
		void Update(double dt);
		void OnInitScene();
		void OnUnInitScene();

		// Clones an audiobuffer to create a voice to the component
		void AddVoice(const std::wstring& name);

		// Playback options of audio
		// Play audio
		void Play(const std::wstring& name);
		// Stop audio
		void Stop(const std::wstring& name);

	private:
		std::map<std::wstring, AudioVoice> m_Voices;
		std::map<std::wstring, bool> m_WasPlaying;

		void pauseToggleAudio(PauseGame* evnt);
	};
}

#endif
