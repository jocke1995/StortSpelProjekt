#ifndef VOICE_H
#define VOICE_H
#include <xaudio2.h>
class Voice
{
public:
	Voice(XAUDIO2_BUFFER* buff, WAVEFORMATEXTENSIBLE* wfxFormat);
	~Voice();

	void Play();
	void Stop();
private:
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;
	XAUDIO2_BUFFER* m_pBuffer = nullptr;
};

#endif