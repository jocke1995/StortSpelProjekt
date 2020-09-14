#ifndef AUDIOCOMPONENT_H
#define AUDIOCOMPONENT_H

#include "Component.h"
class Audio;
class AudioEngine;

namespace component
{
	class AudioComponent : public Component
	{
	public:
		AudioComponent(Entity* parent);
		virtual ~AudioComponent();
		void Update(double dt);

		//AudioEngine* GetAudioEngine() const;

		//void AddAudio(std::string handle, std::string path);
		void AddAudio(Audio* audio);
		//std::vector<std::string>* GetAudioHandles();

		void PlayAudio(unsigned int i = 0);
		void StopAudio(unsigned int i = 0);
		//void PlayAudio(std::string handle);
		//void StopAudio(std::string handle);
		void SetVolume();

	private:
		Audio* m_pAudio = nullptr;
		std::vector<Audio*> m_Audios;
		//AudioEngine* m_pAudioEngine;
		// vector that keeps handles for all the sounds of the audiocomponent
		//std::vector<std::string> m_HandleBank;
	};
}





#endif