#include "stdafx.h"
#include "Audio3DEmitterComponent.h"
#include "../AudioEngine/AudioEngine.h"
#include "../AudioEngine/AudioVoice.h"
#include "../Misc/AssetLoader.h"
#include "../Renderer/Transform.h"
#include "TransformComponent.h"
#include "../Entity.h"

component::Audio3DEmitterComponent::Audio3DEmitterComponent(Entity* parent) : Component(parent)
{
	// 3D Emitter settings, these values need to be set at initialization, rest will be updated later
	m_Emitter = { 0 };
	m_Emitter.ChannelCount = 1;
	m_Emitter.CurveDistanceScaler = FLT_MIN;
	//m_Emitter.CurveDistanceScaler = FLT_MAX;

	// temporary emitter orientation
	DirectX::XMFLOAT3 tempFloat;
	tempFloat.x = 0.0;
	tempFloat.y = 0.0;
	tempFloat.z = 0.0;
	m_Emitter.OrientFront = tempFloat;

	tempFloat.y = 1.0;
	m_Emitter.OrientTop = tempFloat;


}

component::Audio3DEmitterComponent::~Audio3DEmitterComponent()
{
}

void component::Audio3DEmitterComponent::Update(double dt)
{
}

void component::Audio3DEmitterComponent::UpdatePosition(const std::wstring &name)
{
	// get parent entity and look for transform components and get their position/orientation and update m_Emitter
	m_pTransform = m_pParent->GetComponent<TransformComponent>()->GetTransform();

	m_Emitter.Position = m_pTransform->GetPositionXMFLOAT3();

	AudioEngine* audioEngine = &AudioEngine::GetInstance();
	X3DAudioCalculate(*audioEngine->GetX3DInstance(), audioEngine->GetListener(), &m_Emitter, X3DAUDIO_CALCULATE_MATRIX, &m_DSPSettings);
	m_Voices[name].GetSourceVoice()->SetOutputMatrix(audioEngine->GetMasterVoice(), 1, audioEngine->GetDeviceDetails()->InputChannels, m_DSPSettings.pMatrixCoefficients);
	//m_Voices[name].GetSourceVoice()->SetFrequencyRatio(audioEngine->Get3DFXSettings()->DopplerFactor); //needs X3DAUDIO_CALCULATE_DOPPLER flag in X3Daudiocalculate function
	//XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * audioEngine->Get3DFXSettings()->LPFDirectCoefficient), 1.0f };
	//m_Voices[name].GetSourceVoice()->SetFilterParameters(&FilterParameters);
	//Log::Print("x: %f, y: %f, z:%f\n", m_Emitter.Position.x, m_Emitter.Position.y, m_Emitter.Position.z);
}

void component::Audio3DEmitterComponent::AddVoice(const std::wstring& name)
{
	if (m_Voices.count(name) == 0)
	{
		m_Voices.insert(std::make_pair(name, AssetLoader::Get()->GetAudio(name)->CloneVoice()));

		matrix = new FLOAT32[AudioEngine::GetInstance().GetDeviceDetails()->InputChannels];
		m_DSPSettings.SrcChannelCount = 1;	// increase this per added voice?
		m_DSPSettings.DstChannelCount = AudioEngine::GetInstance().GetDeviceDetails()->InputChannels;
		m_DSPSettings.pMatrixCoefficients = matrix;
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
