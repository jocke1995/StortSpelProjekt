#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H
#include "AudioEngine.h"
// Helper functions to potentially be used later
//#define XAUDIO2_HELPER_FUNCTIONS

// Defines used for loading of files, they are written in reverse because of "Little-Endian" format
// XAudio2 uses the Resource Interchange File Format (RIFF). 
// "https://docs.microsoft.com/en-us/windows/win32/xaudio2/resource-interchange-file-format--riff-" 
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#include "AudioVoice.h"

// AudioBuffer is used for loading of wav files and holding buffer data.
// This is then cloned for use with separate entities, both 2D and 3D sound.
class AudioBuffer
{
public:
	AudioBuffer();
	AudioBuffer(const std::wstring& path);
	~AudioBuffer();

	//void OpenFile(IXAudio2* pXAudio2, const TCHAR* path);
	void OpenFile(IXAudio2* pXAudio2, const std::wstring& path);

	//Set how many times audio should loop. 0 = "infinte"; 1 = "play once"; 1> = "play multiple"
	void SetAudioLoop(int loopCount);

	// Creates a voice object which may play the loaded audio.
	AudioVoice CloneVoice();

private:
	friend class AudioVoice;
	// Voices are audio channels that can be routed to and/or combined in IXAudio2SubmixVoice and IXAudio2MasteringVoice.
	// Submix and Master voices mix the audio from all voices feeding into them and operate on the result.
	IXAudio2SourceVoice* m_pSourceVoice;

	// Struct that contains "fmt chunk" from loaded file (See RIFF docs)
	WAVEFORMATEXTENSIBLE m_Wfx;
	// Struct that contains "data chunk" from loaded file (See RIFF docs)
	XAUDIO2_BUFFER m_Buffer;

	HRESULT m_Hr;

	// Function used in OpenFile that finds "chunks" in RIFF files
	HRESULT findChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	// Function used in OpenFile that reads "chunks"
	HRESULT readChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
};


#endif
