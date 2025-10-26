#pragma once
#include "System/Scene.h"
namespace Sample {
	USING_PTR(DiceScene);

	class SampleAnotherScene :
		public Scene
	{
	public:
		USING_SUPER(SampleAnotherScene);
		void Load() override;
		int Init()override;
		void Update() override;
		void LateDraw() override;
		void UnLoad() override;
		SafeSharedPtr<AudioClip> bgm = nullptr;
		DiceSceneWP dice_scene;
		SafeSharedPtr<Texture> title_img = nullptr;
		std::vector<int> results;
		CameraWP cam;
	};
}

