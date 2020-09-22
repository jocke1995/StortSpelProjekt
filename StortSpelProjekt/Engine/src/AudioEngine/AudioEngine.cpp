#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
	m_pXAudio2 = nullptr;
	m_pMasterVoice = nullptr;

	HRESULT hr;
	if (FAILED(hr = XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		Log::Print("Failed to create audio engine\n");
	}

	if (FAILED(hr = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice)))
	{
		Log::Print("Failed to create mastering voice\n");
	}

	// 3D initialization
	m_pMasterVoice->GetChannelMask(&m_DwChannelMask);
	X3DAudioInitialize(m_DwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);
	m_pMasterVoice->GetVoiceDetails(&m_DeviceDetails);

	// debug info
	//m_AudioDebugInfo.TraceMask = XAUDIO2_LOG_ERRORS;
	m_pXAudio2->SetDebugConfiguration(&m_AudioDebugInfo);
}

AudioEngine& AudioEngine::GetInstance()
{
	static AudioEngine instance;
	return instance;
}

AudioEngine::~AudioEngine()
{
	m_pMasterVoice->DestroyVoice();
	m_pXAudio2->Release();
}

IXAudio2* AudioEngine::GetAudioEngine()
{
	return m_pXAudio2;
}

X3DAUDIO_HANDLE* AudioEngine::GetX3DInstance()
{
	return &m_X3DInstance;
}

IXAudio2MasteringVoice* AudioEngine::GetMasterVoice()
{
	return m_pMasterVoice;
}

XAUDIO2_VOICE_DETAILS* AudioEngine::GetDeviceDetails()
{
	return &m_DeviceDetails;
}

void AudioEngine::SetListenerPtr(X3DAUDIO_LISTENER* listener)
{
	m_pListener = listener;
}

X3DAUDIO_LISTENER* AudioEngine::GetListener()
{
	return m_pListener;
}
