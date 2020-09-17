#include "AudioEngine.h"
#include "AudioBuffer.h"
#include "../ECS/Entity.h"

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

	m_pMasterVoice->GetVoiceDetails(&deviceDetails);
	/*matrix = new FLOAT32[deviceDetails.InputChannels];
	m_3DFXSettings.SrcChannelCount = 1;
	m_3DFXSettings.DstChannelCount = deviceDetails.InputChannels;
	m_3DFXSettings.pMatrixCoefficients = matrix;*/

	m_AudioDebugInfo.TraceMask = XAUDIO2_LOG_ERRORS;
	m_AudioDebugInfo.LogFileline = true;
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
	//delete matrix;
}

IXAudio2* AudioEngine::GetAudioEngine()
{
	return m_pXAudio2;
}

X3DAUDIO_HANDLE* AudioEngine::GetX3DInstance()
{
	return &m_X3DInstance;
}

void AudioEngine::SetListener(DirectX::XMFLOAT3 orientFront, DirectX::XMFLOAT3 orientTop, DirectX::XMFLOAT3 position)
{
	m_Listener.OrientFront = orientFront;
	m_Listener.OrientTop = orientTop;
	//// left handed to right handed coordinates z-change
	//DirectX::XMFLOAT3 rhz;
	//rhz.x = position.x;
	//rhz.y = position.y;
	//rhz.z = -position.z;
	//m_Listener.Position = rhz;
	m_Listener.Position = position;

	//Log::Print("x: %f, y: %f, z:%f\n", m_Listener.Position.x, m_Listener.Position.y, m_Listener.Position.z);
}

X3DAUDIO_LISTENER* AudioEngine::GetListener()
{
	return &m_Listener;
}

//X3DAUDIO_DSP_SETTINGS* AudioEngine::GetDSPSettings()
//{
//	return &m_DSPSettings;
//}

IXAudio2MasteringVoice* AudioEngine::GetMasterVoice()
{
	return m_pMasterVoice;
}

XAUDIO2_VOICE_DETAILS* AudioEngine::GetDeviceDetails()
{
	return &deviceDetails;
}
