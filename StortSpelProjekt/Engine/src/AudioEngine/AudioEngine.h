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
	IXAudio2* m_pXAudio2;
	IXAudio2MasteringVoice* m_pMasterVoice;
};

#endif