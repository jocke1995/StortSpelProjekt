#include "stdafx.h"
#include "AudioComponent.h"
#include "../AudioEngine/AudioEngine.h"


component::AudioComponent::AudioComponent(Entity* parent, AudioEngine* audioEngine)
	:Component(parent)
{
	//m_pAudio = new Audio();
	//m_pAudio->OpenFile();
	//PlayAudio();
	m_pAudioEngine = audioEngine;
}

component::AudioComponent::~AudioComponent()
{
	//delete m_pAudioEngine;
}

void component::AudioComponent::Update(double dt)
{
	// may later be used to update positions of sounds?	
}

void component::AudioComponent::AddAudio(std::string handle, std::string path)
{
	// send handle and path to the audioEngine
	m_pAudioEngine->AddAudioBank(handle, path);






	// save handle in vector so that we can have multiple sounds attached
	m_HandleBank.push_back(handle);
}

std::vector<std::string>* component::AudioComponent::GetAudioHandles()
{
	return &m_HandleBank;
}

void component::AudioComponent::PlayAudio(std::string handle)
{
	m_pAudioEngine->PlayAudio(handle);
}

void component::AudioComponent::StopAudio(std::string handle)
{
	m_pAudioEngine->StopAudio(handle);
}
