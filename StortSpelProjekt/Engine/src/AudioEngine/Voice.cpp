#include "stdafx.h"
#include "Voice.h"
#include "../AudioEngine/AudioEngine.h"
Voice::Voice(XAUDIO2_BUFFER* buff, WAVEFORMATEXTENSIBLE* wfxFormat)
{
	m_pBuffer = buff;

    if (FAILED(AudioEngine::GetInstance().GetAudioEngine()->CreateSourceVoice(&m_pSourceVoice, (WAVEFORMATEX*)wfxFormat)))
    {
        Log::Print("Failed creating sound source\n");
    }

    if (FAILED(m_pSourceVoice->SubmitSourceBuffer(m_pBuffer)))
    {
        Log::Print("Failed to submit source buffer\n");
    }

    m_pSourceVoice->SetVolume(0.1);
}

Voice::~Voice()
{
}

void Voice::Play()
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

void Voice::Stop()
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
