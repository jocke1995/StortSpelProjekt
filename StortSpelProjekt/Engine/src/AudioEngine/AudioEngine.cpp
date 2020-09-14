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