#pragma once
#include "System/Scene.h"
USING_PTR(SampleObject);
USING_PTR(AudioPlayer);
USING_PTR(Model);
class SceneSample :
	public Scene
{
public:
	USING_SUPER(SceneSample);
	void Load() override;
	int Init() override;
	void Update() override;
	void PreDraw() override;
	void Draw() override;
	void LateDraw() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	void Exit() override;
	void UnLoad() override;
	SampleObjectWP obj;
	int handle_d4 = -1;
	int handle_d6 = -1;
	int handle_d8 = -1;
	int handle_d10 = -1;
	int handle_d12 = -1;
	int handle_d20 = -1;
	physx::PxActor* plane = nullptr;
	SafeSharedPtr<AudioClip> audio = nullptr;
	SafeSharedPtr<AudioClip> audio2 = nullptr;
	SafeSharedPtr<AudioClip> bgm = nullptr;
	AudioPlayerWP audio_player;

	ObjectWP obj2;
	ObjectWP obj3;
};