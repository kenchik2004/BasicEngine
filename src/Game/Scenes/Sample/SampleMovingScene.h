#pragma once
#include "System/Scene.h"


namespace Sample {
	class SampleMovingScene :
		public Scene
	{
	public:
		USING_SUPER(SampleMovingScene);
		void Load() override;
		int Init() override;
		void Update() override;
		void PreDraw() override;
		void LateDraw() override;
		void Exit() override;

	private:
		bool CheckForLoading(); //!< ロード状況の確認
		GameObjectWP player;
		GameObjectWP camera;
		float camera_distance = 20.0f;
		float camera_rot_y = 0.0f;
	};
}
