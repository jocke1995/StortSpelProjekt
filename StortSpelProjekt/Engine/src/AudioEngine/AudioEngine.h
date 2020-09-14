#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include <xaudio2.h>
class Audio;
class Entity;

class AudioEngine
{
public:
	static AudioEngine& GetInstance();
	~AudioEngine();

	IXAudio2* GetAudioEngine();

	// Adds audio handle and its path to m_AudioBank
	void AddAudioBank(std::string handle, std::string path);

	// Loads required audio files to be ready to be played in scene
	void LoadAudioFiles(Entity* entity);
	// Unloads audio files no longer required in scene (audiobank will still keep a record of files, ready to load back again if needed)
	void UnloadAudioFiles();

	// Playback
	void PlayAudio(std::string handle);
	void StopAudio(std::string handle);

private:
	AudioEngine();
	// The "engine" interface for XAudio2 
	IXAudio2* m_pXAudio2;
	// Voice that represents the audio output device, this is where all other voices will are routed to be heard.
	IXAudio2MasteringVoice* m_pMasterVoice;

	Audio* m_pAudio;
	std::map<std::string, std::string> m_audioBank;
	std::map<std::string, Audio*> m_LoadedAudioFiles;
	//std::vector<Audio> m_pLoadedAudioFiles;
};

#endif