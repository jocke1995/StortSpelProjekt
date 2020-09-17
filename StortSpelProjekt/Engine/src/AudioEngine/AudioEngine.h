#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include <xaudio2.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2")
//#define XAUDIO2_HELPER_FUNCTIONS



class AudioBuffer;
class Entity;

class AudioEngine
{
public:
	static AudioEngine& GetInstance();
	~AudioEngine();

	IXAudio2* GetAudioEngine();
	X3DAUDIO_HANDLE* GetX3DInstance();
	// set listener structure OrientFront, OrientTop, Position
	void SetListener(DirectX::XMFLOAT3 orientFront, DirectX::XMFLOAT3 orientTop, DirectX::XMFLOAT3 position);
	X3DAUDIO_LISTENER* GetListener();
	//X3DAUDIO_DSP_SETTINGS* GetDSPSettings();
	IXAudio2MasteringVoice* GetMasterVoice();
	XAUDIO2_VOICE_DETAILS* GetDeviceDetails();


private:
	AudioEngine();
	// The "engine" interface for XAudio2 
	IXAudio2* m_pXAudio2;
	// AudioVoice that represents the audio output device, this is where all other voices will are routed to be heard.
	IXAudio2MasteringVoice* m_pMasterVoice;

	// 3d stuff
	DWORD m_DwChannelMask;
	X3DAUDIO_HANDLE m_X3DInstance;

	// 3D audio Listener struct, this contains world coordinates and orientation for the "listener" of 3D audio
	X3DAUDIO_LISTENER m_Listener = { };
	//// structure needed for 3D audio and DSP (digital signal processing) effects, holds values returned from x3dAudioCalculate
	//X3DAUDIO_DSP_SETTINGS m_DSPSettings = { 0 };
	//// matrix coefficients for m_DSPSettings
	//FLOAT32* matrix;
	XAUDIO2_VOICE_DETAILS deviceDetails;

	XAUDIO2_DEBUG_CONFIGURATION m_AudioDebugInfo;
};

#endif