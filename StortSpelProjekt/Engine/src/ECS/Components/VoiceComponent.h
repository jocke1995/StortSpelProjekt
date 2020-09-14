#ifndef VOICECOMPONENT_H
#define VOICECOMPONENT_H

#include "Component.h"
class Voice;
class AudioEngine;

// Component used for playing sounds

namespace component
{
	class VoiceComponent : public Component
	{
	public:
		VoiceComponent(Entity* parent);
		virtual ~VoiceComponent();
		void Update(double dt);

		void AddVoice(const std::wstring& name);

		void PlayVoice(const std::wstring& name);
		void StopVoice(const std::wstring& name);

	private:
		std::map<std::wstring, Voice> m_Voices;
	};
}

#endif