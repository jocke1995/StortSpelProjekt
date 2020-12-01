#include "stdafx.h"
#include "Audio3DEmitterComponent.h"
#include "../AudioEngine/AudioVoice.h"
#include "../Misc/AssetLoader.h"
#include "../Renderer/Transform.h"
#include "../Entity.h"

component::Audio3DEmitterComponent::Audio3DEmitterComponent(Entity* parent) : Component(parent)
{
}

component::Audio3DEmitterComponent::~Audio3DEmitterComponent()
{
	for (auto audio : m_VoiceEmitterData)
	{
		delete audio.second.DSPSettings.pMatrixCoefficients;
		delete audio.second.emitter.pChannelAzimuths;
		audio.second.voice.GetSourceVoice()->DestroyVoice();
	}
}

void component::Audio3DEmitterComponent::Update(double dt)
{
}

void component::Audio3DEmitterComponent::OnInitScene()
{
}

void component::Audio3DEmitterComponent::OnUnInitScene()
{
}

void component::Audio3DEmitterComponent::UpdateEmitter(const std::wstring& name)
{
	if (m_VoiceEmitterData.count(name) != 0)
	{
		// get parent entity and look for transform components and get update position
		m_pTransform = m_pParent->GetComponent<TransformComponent>()->GetTransform();
		m_VoiceEmitterData[name].emitter.Position = m_pTransform->GetPositionXMFLOAT3();
		// update front and top vectors
		DirectX::XMMATRIX rotMat = m_pTransform->GetRotMatrix();
		DirectX::XMFLOAT3 forward, up;
		DirectX::XMStoreFloat3(&forward, rotMat.r[2]);
		DirectX::XMStoreFloat3(&up, rotMat.r[1]);
		m_VoiceEmitterData[name].emitter.OrientFront = forward;
		m_VoiceEmitterData[name].emitter.OrientTop = up;

		// calculates values relative to the emitter and listener and saves values in emitter.DSPSettings
		X3DAudioCalculate(
			*AudioEngine::GetInstance().GetX3DInstance(),
			AudioEngine::GetInstance().GetListener(),
			&m_VoiceEmitterData[name].emitter,
			X3DAUDIO_CALCULATE_MATRIX,
			&m_VoiceEmitterData[name].DSPSettings);

		// temporary solution due to strange behaviour of the way matrixcoefficients are saved
		float temp = m_VoiceEmitterData[name].DSPSettings.pMatrixCoefficients[1];
		m_VoiceEmitterData[name].DSPSettings.pMatrixCoefficients[1] = m_VoiceEmitterData[name].DSPSettings.pMatrixCoefficients[2];
		m_VoiceEmitterData[name].DSPSettings.pMatrixCoefficients[2] = temp;

		// sets output matrix for the sound according to the matrix coefficients calculated earlier (changes the soundchannel levels)
		m_VoiceEmitterData[name].voice.GetSourceVoice()->SetOutputMatrix(
			AudioEngine::GetInstance().GetMasterVoice(),
			m_VoiceEmitterData[name].voiceDetails.InputChannels,
			AudioEngine::GetInstance().GetDeviceDetails()->InputChannels,
			m_VoiceEmitterData[name].DSPSettings.pMatrixCoefficients);
	}
}

void component::Audio3DEmitterComponent::AddVoice(const std::wstring& name)
{
	if (m_VoiceEmitterData.count(name) == 0)
	{
		EmitterData emitterData;
		// add new sound and get details
		emitterData.voice = AssetLoader::Get()->GetAudio(name)->CloneVoice();
		if (emitterData.voice.GetSourceVoice() != nullptr)
		{
			emitterData.voice.GetSourceVoice()->GetVoiceDetails(&emitterData.voiceDetails);
			// set emitter settings for 3d audio calculations
			emitterData.emitter = { 0 };
			emitterData.emitter.ChannelCount = emitterData.voiceDetails.InputChannels;
			emitterData.emitter.CurveDistanceScaler = 1.0f;
			emitterData.emitter.OrientFront = DirectX::XMFLOAT3(0.0, 0.0, 0.0); // direction the sound will be facing, this is later updated to parent entity direction
			emitterData.emitter.OrientTop = DirectX::XMFLOAT3(0.0, 1.0, 0.0); // up vector, this is later updated to change with the parent entitys up vector
			emitterData.emitter.ChannelRadius = 0.0f;
			emitterData.emitter.pChannelAzimuths = new FLOAT32[emitterData.voiceDetails.InputChannels];
			emitterData.emitter.pChannelAzimuths[0] = X3DAUDIO_PI / 4;
			emitterData.emitter.pChannelAzimuths[1] = 7 * X3DAUDIO_PI / 4;
			// prepare DSPSettings according to the voice details
			emitterData.DSPSettings.SrcChannelCount = emitterData.voiceDetails.InputChannels;
			emitterData.DSPSettings.DstChannelCount = AudioEngine::GetInstance().GetDeviceDetails()->InputChannels;
			int coefficients = emitterData.DSPSettings.SrcChannelCount * emitterData.DSPSettings.DstChannelCount;
			emitterData.DSPSettings.pMatrixCoefficients = new FLOAT32[coefficients];

			// put emitterData struct in the map with key
			m_VoiceEmitterData.emplace(std::make_pair(name, emitterData));
		}
	}
}

void component::Audio3DEmitterComponent::AddVoice(const std::wstring& globalName, const std::wstring& localName)
{
	if (m_VoiceEmitterData.count(localName) == 0)
	{
		EmitterData emitterData;
		// add new sound and get details
		emitterData.voice = AssetLoader::Get()->GetAudio(globalName)->CloneVoice();
		if (emitterData.voice.GetSourceVoice() != nullptr)
		{
			emitterData.voice.GetSourceVoice()->GetVoiceDetails(&emitterData.voiceDetails);
			// set emitter settings for 3d audio calculations
			emitterData.emitter = { 0 };
			emitterData.emitter.ChannelCount = emitterData.voiceDetails.InputChannels;
			emitterData.emitter.CurveDistanceScaler = 1.0f;
			emitterData.emitter.OrientFront = DirectX::XMFLOAT3(0.0, 0.0, 0.0); // direction the sound will be facing, this is later updated to parent entity direction
			emitterData.emitter.OrientTop = DirectX::XMFLOAT3(0.0, 1.0, 0.0); // up vector, this is later updated to change with the parent entitys up vector
			emitterData.emitter.ChannelRadius = 0.0f;
			emitterData.emitter.pChannelAzimuths = new FLOAT32[emitterData.voiceDetails.InputChannels];
			emitterData.emitter.pChannelAzimuths[0] = X3DAUDIO_PI / 4;
			emitterData.emitter.pChannelAzimuths[1] = 7 * X3DAUDIO_PI / 4;
			// prepare DSPSettings according to the voice details
			emitterData.DSPSettings.SrcChannelCount = emitterData.voiceDetails.InputChannels;
			emitterData.DSPSettings.DstChannelCount = AudioEngine::GetInstance().GetDeviceDetails()->InputChannels;
			int coefficients = emitterData.DSPSettings.SrcChannelCount * emitterData.DSPSettings.DstChannelCount;
			emitterData.DSPSettings.pMatrixCoefficients = new FLOAT32[coefficients];

			// put emitterData struct in the map with key
			m_VoiceEmitterData.emplace(std::make_pair(localName, emitterData));
		}
	}
}

void component::Audio3DEmitterComponent::Play(const std::wstring& name)
{
	if (m_VoiceEmitterData.count(name) != 0)
	{
		m_VoiceEmitterData[name].voice.Play();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Tried to play Audio that has not been loaded: %S \n", name.c_str());
	}
}

void component::Audio3DEmitterComponent::PlayDontReset(const std::wstring& name)
{
	m_VoiceEmitterData[name].voice.PlayDontReset();
}

void component::Audio3DEmitterComponent::Stop(const std::wstring& name)
{
#ifdef _DEBUG
	if (m_VoiceEmitterData.count(name) != 0)
	{
		m_VoiceEmitterData[name].voice.Stop();
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Tried to stop Audio that has not been loaded: %S \n", name.c_str());
	}
#else
	m_VoiceEmitterData[name].voice.Stop();
#endif
}
