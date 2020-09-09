#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
	m_pXAudio2 = nullptr;
	m_pMasterVoice = nullptr;

	HRESULT hr;
	//CoInitialize(nullptr);
	if (FAILED(hr = XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		Log::Print("Failed to create audio engine\n");
	}

	if (FAILED(hr = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice)))
	{
		Log::Print("Failed to create mastering voice\n");
	}
	//Log::Print("CREATED SOME SHET\n");
}

AudioEngine::~AudioEngine()
{
}

IXAudio2* AudioEngine::GetAudioEngine()
{
	return m_pXAudio2;
}