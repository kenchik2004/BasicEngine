#pragma once
#include "System/Scene.h"
USING_PTR(CameraObject);
namespace NeonFade {


	class SceneGame :
		public Scene
	{
	public:
		USING_SUPER(SceneGame);
		void Load() override;
		int Init() override;
		void Update() override;
		void PreDraw() override;
		void LateDebugDraw() override;
		void OnLateDrawFinish() override;

		void Exit() override;
		bool CheckForLoading();
	private:
		CameraObjectWP camera;
		GameObjectWP player;

	};
}

