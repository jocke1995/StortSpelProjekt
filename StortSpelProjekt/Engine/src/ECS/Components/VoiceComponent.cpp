#include "stdafx.h"
#include "VoiceComponent.h"
#include "../AudioEngine/Voice.h"
#include "../Misc/AssetLoader.h"

component::VoiceComponent::VoiceComponent(Entity* parent): Component(parent)
{
}

component::VoiceComponent::~VoiceComponent()
{
}

void component::VoiceComponent::Update(double dt)
{
}

void component::VoiceComponent::AddVoice(const std::wstring& name)
{
	if (m_Voices.count(name) == 0)
	{
		m_Voices.insert(std::make_pair(name, AssetLoader::Get()->GetAudio(name)->CloneVoice()));
	}
}

void component::VoiceComponent::PlayVoice(const std::wstring& name)
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

void component::VoiceComponent::StopVoice(const std::wstring& name)
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
