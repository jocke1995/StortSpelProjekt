#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include <xaudio2.h>

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();

	IXAudio2* GetAudioEngine();

private:
	// The "engine" interface for XAudio2 
	IXAudio2* m_pXAudio2;
	// Voice that represents the audio output device, this is where all other voices will are routed to be heard.
	IXAudio2MasteringVoice* m_pMasterVoice;
};

#endif