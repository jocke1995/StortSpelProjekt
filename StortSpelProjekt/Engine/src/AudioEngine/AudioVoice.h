#ifndef AUDIOVOICE_H
#define AUDIOVOICE_H
#include <xaudio2.h>
class AudioBuffer;

/// <summary>
/// Audiosource, plays sounds on command.
/// </summary>
class AudioVoice
{
public:
	/// <summary>
	/// Default constructor, avoid if possible. Object needs to be initialized before use!
	/// </summary>
	AudioVoice();
	/// <summary>
	/// Regular constructor.
	/// </summary>
	AudioVoice(AudioBuffer& audio);
	~AudioVoice();

	/// <summary>
	/// Initializer, needs to be called before use if default constructor was used.
	/// Makes a shallow copy of the audiobuffer and creates the new sourcevoice.
	/// </summary>
	void Initialize(AudioBuffer& audio);

	IXAudio2SourceVoice* GetSourceVoice();

	/// <summary>
	/// Plays audio.
	/// </summary>
	void Play();
	/// <summary>
	/// Stops playing audio.
	/// </summary>
	void Stop();

	/// <summary>
	/// Resumes playing paused audio.
	/// </summary>
	void Resume();
	/// <summary>
	/// Pauses audio.
	/// </summary>
	void Pause();
	
private:
	bool m_Initialized = true;
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;
	XAUDIO2_BUFFER* m_pBuffer = nullptr;
	WAVEFORMATEXTENSIBLE* m_pWfxFormat = nullptr;

	/// <summary>
	/// Initializer, called by regular constructor.
	/// Makes a shallow copy of the audiobuffer and creates the new sourcevoice.
	/// </summary>
	void initialize(XAUDIO2_BUFFER* buff, WAVEFORMATEXTENSIBLE* wfxFormat);
};

#endif
