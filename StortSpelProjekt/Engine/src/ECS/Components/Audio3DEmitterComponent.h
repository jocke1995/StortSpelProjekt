#ifndef AUDIO3DEMITTERCOMPONENT_H
#define AUDIO3DEMITTERCOMPONENT_H

#include "Component.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include <Windows.h>
#include "../AudioEngine/AudioVoice.h"

class AudioVoice;
class Transform;

// struct that holds all data needed for separate sounds
struct EmitterData
{
	AudioVoice voice;
	XAUDIO2_VOICE_DETAILS voiceDetails;
	X3DAUDIO_EMITTER emitter;
	X3DAUDIO_DSP_SETTINGS DSPSettings = { 0 };
};

// Component used for setting/updating "Emitters" position and orientation in 3D audio playback
namespace component
{
	class Audio3DEmitterComponent : public Component
	{
	public:
		Audio3DEmitterComponent(Entity* parent);
		virtual ~Audio3DEmitterComponent();

		// update position and orientation of the emitter
		void UpdateEmitter(const std::wstring& name);

		// Clones an audiobuffer to create a voice to the component
		void AddVoice(const std::wstring& name);

		// Playback options of audio
		// Play audio
		void Play(const std::wstring& name);
		// Stop audio
		void Stop(const std::wstring& name);

	private:
		std::map<std::wstring, EmitterData> m_VoiceEmitterData;
		Transform* m_pTransform = nullptr;
	};
}

#endif
