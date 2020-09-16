#ifndef AUDIOVOICE_H
#define AUDIOVOICE_H
#include <xaudio2.h>
//#include <x3daudio.h>
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
private:
	bool m_Initialized = true;
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;
	XAUDIO2_BUFFER* m_pBuffer = nullptr;
	WAVEFORMATEXTENSIBLE* m_pWfxFormat = nullptr;

	// 3d
	//bool m_Listener = false;
	// Emitter sets world positions of the audio source to be calculated for 3D sound
	//X3DAUDIO_EMITTER m_Emitter;

	/// <summary>
	/// Initializer, called by regular constructor.
	/// Makes a shallow copy of the audiobuffer and creates the new sourcevoice.
	/// </summary>
	void initialize(XAUDIO2_BUFFER* buff, WAVEFORMATEXTENSIBLE* wfxFormat);
};

#endif