#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include <xaudio2.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2")

class AudioEngine
{
public:
	static AudioEngine& GetInstance();
	~AudioEngine();

	IXAudio2* GetAudioEngine();
	X3DAUDIO_HANDLE* GetX3DInstance();
	IXAudio2MasteringVoice* GetMasterVoice();
	XAUDIO2_VOICE_DETAILS* GetDeviceDetails();
	void SetListenerPtr(X3DAUDIO_LISTENER* listener);
	X3DAUDIO_LISTENER* GetListener();
	void ChangeMasterVolume(float vol);

private:
	AudioEngine();
	// The "engine" interface for XAudio2 
	IXAudio2* m_pXAudio2;
	// AudioVoice that represents the audio output device, this is where all other voices will are routed to be heard.
	IXAudio2MasteringVoice* m_pMasterVoice;

	// 3D stuff
	DWORD m_DwChannelMask;
	X3DAUDIO_HANDLE m_X3DInstance;
	// Pointer to listener struct, put here for ease of access for emittercomponent class. Original listener struct contained in Audio3DListenerComponent
	X3DAUDIO_LISTENER* m_pListener;
	// contains details about the sound device
	XAUDIO2_VOICE_DETAILS m_DeviceDetails;
	// for debugging purposes
	XAUDIO2_DEBUG_CONFIGURATION m_AudioDebugInfo;
};

#endif
