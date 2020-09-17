#include "stdafx.h"
#include "AudioVoice.h"
#include "AudioBuffer.h"
//#include "AudioEngine.h"

AudioVoice::AudioVoice()
{
    m_Initialized = false;
}

AudioVoice::AudioVoice(AudioBuffer& audio)
{
    initialize(&audio.m_Buffer, &audio.m_Wfx);
}

AudioVoice::~AudioVoice()
{
}

void AudioVoice::initialize(XAUDIO2_BUFFER* buff, WAVEFORMATEXTENSIBLE* wfxFormat)
{
    m_pBuffer = buff;
    m_pWfxFormat = wfxFormat;
    if (FAILED(AudioEngine::GetInstance().GetAudioEngine()->CreateSourceVoice(&m_pSourceVoice, (WAVEFORMATEX*)m_pWfxFormat)))
    {
        Log::Print("Failed creating sound source\n");
    }

    if (FAILED(m_pSourceVoice->SubmitSourceBuffer(m_pBuffer)))
    {
        Log::Print("Failed to submit source buffer\n");
    }

    m_pSourceVoice->SetVolume(1.0);
    HRESULT hRes = m_pSourceVoice->SetOutputVoices(NULL);

    //// 3D Emitter settings, these values need to be set at initialization, rest will be updated later
    //m_Emitter = { 0 };
    //m_Emitter.ChannelCount = 1;
    //m_Emitter.CurveDistanceScaler = FLT_MIN;
}

void AudioVoice::Initialize(AudioBuffer& audio)
{
    initialize(&audio.m_Buffer, &audio.m_Wfx);
}

IXAudio2SourceVoice* AudioVoice::GetSourceVoice()
{
    return m_pSourceVoice;
}

void AudioVoice::Play()
{
    if (m_Initialized)
    {
        // reset the buffer if sound has ended
        XAUDIO2_VOICE_STATE test;
        m_pSourceVoice->GetState(&test);
        if (test.BuffersQueued == 0)
        {
            Stop();
        }

        // play the sound
        if (FAILED(m_pSourceVoice->Start(0)))
        {
            Log::Print("Error playing audio\n");
        }
    }
}

void AudioVoice::Stop()
{
    if (m_Initialized)
    {
        // stop playback
        if (FAILED(m_pSourceVoice->Stop(0)))
        {
            Log::Print("Error stopping audio\n");
        }
        // reset the buffer so the sound starts from the beginning at next playback
        m_pSourceVoice->FlushSourceBuffers();
        m_pSourceVoice->SubmitSourceBuffer(m_pBuffer);
    }
}