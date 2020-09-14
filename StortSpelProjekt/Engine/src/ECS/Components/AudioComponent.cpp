#include "stdafx.h"
#include "AudioComponent.h"
//#include "../AudioEngine/AudioEngine.h"
#include "../AudioEngine/Audio.h"
#include "VoiceComponent.h"

component::AudioComponent::AudioComponent(Entity* parent)
	:Component(parent)
{
	//m_pAudio = new Audio();
	//m_pAudio->OpenFile();
	//PlayAudio();
	
	//m_pAudioEngine = &AudioEngine::GetInstance();
}

component::AudioComponent::~AudioComponent()
{
	//delete m_pAudioEngine;
}

void component::AudioComponent::Update(double dt)
{
	// may later be used to update positions of sounds?	
}

//void component::AudioComponent::AddAudio(std::string handle, std::string path)
//{
//	// send handle and path to the audioEngine
//	m_pAudioEngine->AddAudioBank(handle, path);
//
//
//
//
//
//
//	// save handle in vector so that we can have multiple sounds attached
//	m_HandleBank.push_back(handle);
//}

void component::AudioComponent::AddAudio(Audio* audio)
{
	m_Audios.push_back(audio);
}

void component::AudioComponent::PlayAudio(unsigned int i)
{
	m_Audios[i]->PlayAudio();
}

void component::AudioComponent::StopAudio(unsigned int i)
{
	m_Audios[i]->StopAudio();
}

//std::vector<std::string>* component::AudioComponent::GetAudioHandles()
//{
//	return &m_HandleBank;
//}
//
//void component::AudioComponent::PlayAudio(std::string handle)
//{
//	m_pAudioEngine->PlayAudio(handle);
//}
//
//void component::AudioComponent::StopAudio(std::string handle)
//{
//	m_pAudioEngine->StopAudio(handle);
//}
