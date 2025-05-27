#pragma once
#include "System/Component.h"
class AudioPlayer :
	public Component
{
public:
	USING_SUPER(AudioPlayer);
	SafeSharedPtr<AudioClip> audio = nullptr;
	void SetAudio(std::string_view name, std::string_view new_name = "");
	inline void SetAudio(SafeSharedPtr<AudioClip> audio_)
	{
		audio = audio_;
		if (!audio)
			return;
		default_frequency = GetFrequencySoundMem(audio->handle);
	}
	inline SafeSharedPtr<AudioClip> GetAudio() {
		return audio;
	}
	inline void Play(float start_pos = 0, int sample_rate = 44100)
	{
		if (audio) {
			SetCurrentPositionSoundMem((long long)(start_pos * sample_rate), audio->handle);
			PlaySoundMem(audio->handle, DX_PLAYTYPE_LOOP, false);
		}
	}
	void PreDraw() override;
	inline void Stop()
	{
		if (audio)
			StopSoundMem(audio->handle);
	}
	static inline void Load(std::string_view path, std::string_view name, bool use_3d = true)
	{
		AudioManager::Load(path, name, use_3d);
	}
	void Exit() override {
		if (audio)
			audio.reset();
	}
	int default_frequency;
	float pitch_rate = 1.0f;
	float radius = 200;
};

