#include "stdafx.h"
#include "VoiceComponent.h"
#include "../AudioEngine/Voice.h"

component::VoiceComponent::VoiceComponent(Entity* parent): Component(parent)
{
}

component::VoiceComponent::~VoiceComponent()
{
}

void component::VoiceComponent::Update(double dt)
{
}

void component::VoiceComponent::AddVoice(Voice* voice)
{
	m_Voices.push_back(voice);
}

void component::VoiceComponent::PlayVoice(unsigned int i)
{
	m_Voices[i]->Play();
}

void component::VoiceComponent::StopVoice(unsigned int i)
{
	m_Voices[i]->Stop();
}
