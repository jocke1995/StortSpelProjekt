#ifndef VOICECOMPONENT_H
#define VOICECOMPONENT_H

#include "Component.h"
class Voice;
class AudioEngine;

namespace component
{
	class VoiceComponent : public Component
	{
	public:
		VoiceComponent(Entity* parent);
		virtual ~VoiceComponent();
		void Update(double dt);

		//AudioEngine* GetAudioEngine() const;

		//void AddAudio(std::string handle, std::string path);
		void AddVoice(Voice* voice);
		//std::vector<std::string>* GetAudioHandles();

		void PlayVoice(unsigned int i = 0);
		void StopVoice(unsigned int i = 0);
		//void PlayAudio(std::string handle);
		//void StopAudio(std::string handle);
		//void SetVolume();

	private:
		std::vector<Voice*> m_Voices;
		//AudioEngine* m_pAudioEngine;
		// vector that keeps handles for all the sounds of the audiocomponent
		//std::vector<std::string> m_HandleBank;
	};
}

#endif