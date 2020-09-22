#include "stdafx.h"
#include "AudioVoiceComponent.h"
#include "../AudioEngine/AudioVoice.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/Option.h"

component::AudioVoiceComponent::AudioVoiceComponent(Entity* parent): Component(parent)
{
}

component::AudioVoiceComponent::~AudioVoiceComponent()
{
	for (auto audio : m_Voices)
	{
		audio.second.GetSourceVoice()->DestroyVoice();
	}
}

void component::AudioVoiceComponent::Update(double dt)
{
}

void component::AudioVoiceComponent::AddVoice(const std::wstring& name)
{
	if (m_Voices.count(name) == 0)
	{
		m_Voices.insert(std::make_pair(name, AssetLoader::Get()->GetAudio(name)->CloneVoice()));
		// Lower the volume of background sound, so 3d sound can be heard in test scene.
		m_Voices[name].GetSourceVoice()->SetVolume(Option::GetInstance().GetVariable("backgroundVolume"));
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
	m_Voices[name].Stop();
#endif
}
