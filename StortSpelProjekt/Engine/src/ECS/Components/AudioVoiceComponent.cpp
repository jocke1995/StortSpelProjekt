#include "stdafx.h"
#include "AudioVoiceComponent.h"
#include "../AudioEngine/AudioVoice.h"
#include "../Misc/AssetLoader.h"

component::AudioVoiceComponent::AudioVoiceComponent(Entity* parent): Component(parent)
{
}

component::AudioVoiceComponent::~AudioVoiceComponent()
{
}

void component::AudioVoiceComponent::Update(double dt)
{
}

void component::AudioVoiceComponent::InitScene(Renderer* renderer)
{
	Log::Print("AudioVoiceComponent InitScene called!\n");

}

void component::AudioVoiceComponent::AddVoice(const std::wstring& name)
{
	if (m_Voices.count(name) == 0)
	{
		m_Voices.insert(std::make_pair(name, AssetLoader::Get()->GetAudio(name)->CloneVoice()));
	}
}

void component::AudioVoiceComponent::Play(const std::wstring& name)
{
#ifdef _DEBUG
	if (m_Voices.count(name) != 0)
	{
		m_Voices[name].Play();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Tried to play Audio that has not been loaded!");
	}
#else
	m_Voices[name].Play();
#endif
}

void component::AudioVoiceComponent::Stop(const std::wstring& name)
{
#ifdef _DEBUG
	if (m_Voices.count(name) != 0)
	{
		m_Voices[name].Stop();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Tried to stop Audio that has not been loaded!");
	}
#else
	m_Voices[name].Play();
#endif
}
