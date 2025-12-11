#pragma once
#include "System/Scene.h"
#include "Game/Utilitys/NeonFade/StateMachines/SceneGameStateMachine.h"
USING_PTR(CameraObject);
namespace NeonFade {
	USING_PTR(SceneGameStateMachine);
	USING_PTR(Player);
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


		void AddEnemyCount(u32 cnt = 1) { enemy_count += cnt; }
		void SubtractEnemyCount(u32 cnt = 1) { enemy_count -= min(cnt, enemy_count); }
		int GetEnemyCount() const { return enemy_count; }
		CameraObjectWP camera;
		PlayerWP player;
		UIObjectWP text_obj;
		UIObjectWP hud_obj;
		TextWP text_comp;
		SceneGameStateMachineUP scene_state_machine = nullptr;
	private:
		u32 enemy_count = 0;
	};
}

