#pragma once
#include "System/Scene.h"
#include "Game/Utilitys/NeonFade/StateMachines/SceneGameStateMachine.h"
USING_PTR(CameraObject);
namespace NeonFade {
	USING_PTR(SceneGameStateMachine);
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


		void AddEnemyCount() { enemy_count++; }
		void SubtractEnemyCount() { enemy_count--; }
		int GetEnemyCount() const { return enemy_count; }
		CameraObjectWP camera;
		GameObjectWP player;
		UIObjectWP text_obj;
		TextWP text_comp;
		SceneGameStateMachineUP scene_state_machine = nullptr;
	private:
		int enemy_count = 0;
	};
}

