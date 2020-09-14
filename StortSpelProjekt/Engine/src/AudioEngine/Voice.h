#ifndef VOICE_H
#define VOICE_H
#include <xaudio2.h>
class Audio;

/// <summary>
/// Audiosource, plays sounds on command.
/// </summary>
class Voice
{
public:
	/// <summary>
	/// Default constructor, avoid if possible. Object needs to be initialized before use!
	/// </summary>
	Voice();
	/// <summary>
	/// Regular constructor.
	/// </summary>
	Voice(Audio& audio);
	~Voice();

	/// <summary>
	/// Initializer, needs to be called before use if default constructor was used.
	/// </summary>
	void Initialize(Audio& audio);

	/// <summary>
	/// Plays audio.
	/// </summary>
	void Play();
	/// <summary>
	/// Stops playing audio.
	/// </summary>
	void Stop();
private:
	bool m_Initialized = true;
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;
	XAUDIO2_BUFFER* m_pBuffer = nullptr;
	WAVEFORMATEXTENSIBLE* m_pWfxFormat = nullptr;

	void initialize(XAUDIO2_BUFFER* buff, WAVEFORMATEXTENSIBLE* wfxFormat);
};

#endif