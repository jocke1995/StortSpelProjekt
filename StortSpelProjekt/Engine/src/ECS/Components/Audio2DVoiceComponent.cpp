#include "stdafx.h"
#include "Audio2DVoiceComponent.h"
#include "../AudioEngine/AudioVoice.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/Option.h"

component::Audio2DVoiceComponent::Audio2DVoiceComponent(Entity* parent): Component(parent)
{
}

component::Audio2DVoiceComponent::~Audio2DVoiceComponent()
{
	for (auto audio : m_Voices)
	{
		if (audio.second.GetSourceVoice() != nullptr)
		{
			audio.second.GetSourceVoice()->DestroyVoice();
		}
	}
}

void component::Audio2DVoiceComponent::Update(double dt)
{
}

void component::Audio2DVoiceComponent::OnInitScene()
{

}

void component::Audio2DVoiceComponent::OnLoadScene()
{

}

void component::Audio2DVoiceComponent::OnUnloadScene()
{

}

void component::Audio2DVoiceComponent::AddVoice(const std::wstring& name)
{
	if (m_Voices.count(name) == 0)
	{
		AudioVoice clonedVoice = AssetLoader::Get()->GetAudio(name)->CloneVoice();
		if (clonedVoice.GetSourceVoice() != nullptr)
		{
			m_Voices.insert(std::make_pair(name, AssetLoader::Get()->GetAudio(name)->CloneVoice()));
			// Lower the volume of background sound, so 3d sound can be heard in test scene.
			m_Voices[name].GetSourceVoice()->SetVolume(std::atof(Option::GetInstance().GetVariable("f_backgroundVolume").c_str()));
		}
	}
}

void component::Audio2DVoiceComponent::Play(const std::wstring& name)
{
#ifdef _DEBUG
	if (m_Voices.count(name) != 0)
	{
		m_Voices[name].Play();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Tried to play Audio that has not been loaded: %S \n", name.c_str());
	}
#else
	m_Voices[name].Play();
#endif
}

void component::Audio2DVoiceComponent::Stop(const std::wstring& name)
{
#ifdef _DEBUG
	if (m_Voices.count(name) != 0)
	{
		m_Voices[name].Stop();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Tried to stop Audio that has not been loaded: %S \n", name.c_str());
	}
#else
	m_Voices[name].Stop();
#endif
}
