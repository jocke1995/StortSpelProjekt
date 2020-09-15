#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include <xaudio2.h>
class AudioBuffer;
class Entity;

class AudioEngine
{
public:
	static AudioEngine& GetInstance();
	~AudioEngine();

	IXAudio2* GetAudioEngine();

private:
	AudioEngine();
	// The "engine" interface for XAudio2 
	IXAudio2* m_pXAudio2;
	// AudioVoice that represents the audio output device, this is where all other voices will are routed to be heard.
	IXAudio2MasteringVoice* m_pMasterVoice;

	AudioBuffer* m_pAudio;
	//std::vector<Audio> m_pLoadedAudioFiles;
};

#endif