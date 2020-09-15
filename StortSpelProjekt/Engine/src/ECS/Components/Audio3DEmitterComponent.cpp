#include "stdafx.h"
#include "Audio3DEmitterComponent.h"
#include "../AudioEngine/AudioEngine.h"
#include "../AudioEngine/AudioVoice.h"
#include "../Misc/AssetLoader.h"

component::Audio3DEmitterComponent::Audio3DEmitterComponent(Entity* parent) : Component(parent)
{
	// 3D Emitter settings, these values need to be set at initialization, rest will be updated later
	m_Emitter = { 0 };
	m_Emitter.ChannelCount = 1;
	m_Emitter.CurveDistanceScaler = FLT_MIN;
}

component::Audio3DEmitterComponent::~Audio3DEmitterComponent()
{
}

void component::Audio3DEmitterComponent::Update(double dt)
{
}

void component::Audio3DEmitterComponent::UpdatePosition()
{
	// get parent entity and look for transform components and get their position/orientation and update m_Emitter
}

void component::Audio3DEmitterComponent::AddVoice(const std::wstring& name)
{
	if (m_Voices.count(name) == 0)
	{
		m_Voices.insert(std::make_pair(name, AssetLoader::Get()->GetAudio(name)->CloneVoice()));
	}
}

void component::Audio3DEmitterComponent::Play(const std::wstring& name)
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

void component::Audio3DEmitterComponent::Stop(const std::wstring& name)
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
