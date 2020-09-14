#include "stdafx.h"
#include "AudioVoice.h"
#include "AudioBuffer.h"
#include "AudioEngine.h"

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

    m_pSourceVoice->SetVolume(0.1);
}

void AudioVoice::Initialize(AudioBuffer& audio)
{
    m_Initialized = true;
    initialize(&audio.m_Buffer, &audio.m_Wfx);
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