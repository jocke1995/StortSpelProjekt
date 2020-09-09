#ifndef AUDIO_H
#define AUDIO_H
#include "AudioEngine.h"
//#include <xaudio2.h>
//#define XAUDIO2_HELPER_FUNCTIONS

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

class Audio
{
public:
	Audio();
	~Audio();

	void OpenFile(IXAudio2* pXAudio2, const TCHAR* sound);
	void PlayAudio();
	void StopAudio();

private:
	IXAudio2SourceVoice* pSourceVoice;
	WAVEFORMATEXTENSIBLE m_Wfx;
	XAUDIO2_BUFFER m_Buffer;
	HRESULT m_Hr;

	HRESULT findChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	HRESULT readChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
};


#endif