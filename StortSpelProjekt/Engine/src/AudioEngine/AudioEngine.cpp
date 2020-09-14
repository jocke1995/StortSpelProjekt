#include "AudioEngine.h"
#include "Audio.h"
#include "../ECS/Entity.h"

AudioEngine::AudioEngine()
{
	m_pXAudio2 = nullptr;
	m_pMasterVoice = nullptr;
	m_pAudio = nullptr;
	//testAudio = new Audio();

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

void AudioEngine::LoadAudioFiles(Entity* entity)
{
	//// look for entities that have audiocomponents
	//component::AudioComponent* audioComp = entity->GetComponent<component::AudioComponent>();
	//if (audioComp != nullptr)
	//{
	//	// get handles of all audio files attached to component
	//	std::vector<std::string>* handles = audioComp->GetAudioHandles();
	//
	//	// for all individual audio files, create a new Audio object and load the files
	//	std::string handle;
	//	std::string path;
	//	for (int i = 0; i < handles->size(); i++)
	//	{
	//		handle = handles->at(i);
	//		path = m_audioBank[handle];
	//		m_pAudio = new Audio();
	//		m_pAudio->OpenFile(m_pXAudio2, path);
	//		m_LoadedAudioFiles.insert(std::make_pair(handle, m_pAudio));
	//	}
	//
	//	// test to load one audio file
	//	//std::string handle = handles->at(0);
	//	//std::string path = m_audioBank[handle];
	//
	//	//testAudio->OpenFile(m_pXAudio2, path);
	//
	//}
	//// if have audiocomponent -> look up audiohandle -> find its path in the AudioBank and load this file
}

void AudioEngine::AddAudioBank(std::string handle, std::string path)
{
	m_audioBank.insert(std::make_pair(handle, path));
}

void AudioEngine::PlayAudio(std::string handle)
{
	m_LoadedAudioFiles[handle]->PlayAudio();
}

void AudioEngine::StopAudio(std::string handle)
{
	m_LoadedAudioFiles[handle]->StopAudio();
}
