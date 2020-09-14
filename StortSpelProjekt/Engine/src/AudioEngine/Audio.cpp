#include "stdafx.h"
#include "Audio.h"

Audio::Audio()
{
    m_pSourceVoice = nullptr;
    m_Wfx = { 0 };
    m_Buffer = { 0 };
    m_Hr = NULL;
}

Audio::Audio(const std::wstring& path)
{
    m_pSourceVoice = nullptr;
    m_Wfx = { 0 };
    m_Buffer = { 0 };
    m_Hr = NULL;

    OpenFile(AudioEngine::GetInstance().GetAudioEngine(), path);
}

Audio::~Audio()
{
    delete m_Buffer.pAudioData;
}

HRESULT Audio::findChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize)
        {
            return S_FALSE;
        }
    }

    return S_OK;
}

HRESULT Audio::readChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

void Audio::OpenFile(IXAudio2* pXAudio2, std::wstring path)
{
    //std::wstring strFileName = to_wstring(path);

    // Open the file
    HANDLE hFile = CreateFile(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        Log::Print("Invalid handle value\n");
    }

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
    {
        Log::Print("Invalid set file pointer\n");
    }

    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    findChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    readChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE)
    {
        Log::Print("Filetype error\n");
    }

    findChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    readChunkData(hFile, &m_Wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    findChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    BYTE* pDataBuffer = new BYTE[dwChunkSize];
    readChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    m_Buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
    m_Buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
    m_Buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

    if (FAILED(m_Hr = pXAudio2->CreateSourceVoice(&m_pSourceVoice, (WAVEFORMATEX*)&m_Wfx)))
    {
        Log::Print("Failed creating sound source\n");
    }

    if (FAILED(m_Hr = m_pSourceVoice->SubmitSourceBuffer(&m_Buffer)))
    {
        Log::Print("Failed to submit source buffer\n");
    }

    // flags to loop the sound, this is a temporary solution
    m_Buffer.Flags = 0;
    m_Buffer.LoopBegin = 0;
    m_Buffer.LoopLength = 0;
    m_Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    StopAudio();

    m_pSourceVoice->SetVolume(0.1);
}

void Audio::PlayAudio()
{
    // reset the buffer if sound has ended
    XAUDIO2_VOICE_STATE test;
    m_pSourceVoice->GetState(&test);
    if (test.BuffersQueued == 0)
    {
        StopAudio();
    }

    // play the sound
    if (FAILED(m_Hr = m_pSourceVoice->Start(0)))
    {
        Log::Print("Error playing audio\n");
    }
}

void Audio::StopAudio()
{
    // stop playback
    if (FAILED(m_Hr = m_pSourceVoice->Stop(0)))
    {
        Log::Print("Error stopping audio\n");
    }
    // reset the buffer so the sound starts from the beginning at next playback
    m_pSourceVoice->FlushSourceBuffers();
    m_pSourceVoice->SubmitSourceBuffer(&m_Buffer);
}

Voice Audio::CloneVoice()
{
    return Voice(*this);
}
