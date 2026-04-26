#pragma once
#include "System/Scene.h"
#include "Game/Utilitys/NeonFade/StateMachines/SceneGameStateMachine.h"
USING_PTR(CameraObject);
USING_PTR(ShadowMapObject);
USING_PTR(LightManager);
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
		void LateDraw() override;
		void OnLateDrawFinish() override;

		void Exit() override;
		bool CheckForLoading();


		void AddEnemyCount(u32 cnt = 1) { enemy_count += cnt; }
		void SubtractEnemyCount(u32 cnt = 1);
		int GetEnemyCount() const { return enemy_count; }
		void ClearAllEnemy();
		CameraObjectWP camera;
		PlayerWP player;
		UIObjectWP text_obj;
		UIObjectWP hud_obj;
		std::unordered_map<std::string, UIObjectWP> ui_texts;
		TextWP text_comp;
		SceneGameStateMachineUP scene_state_machine = nullptr;
		static constexpr float GAME_TIMER_MAX = 300.0f;

		ShadowMapObjectWP shadowmap = nullptr;
		LightManagerWP light_manager = nullptr;
		void StartGameTimer() { is_game_timer_started = true; }
		void ResetGameTimer() { game_timer = 0.0f;}
		void StopGameTimer() { is_game_timer_started = false; }
		float GetGameTimer() const { return game_timer; }
		bool IsEffectExsist() const;
		bool IsEffectPreparing() const;
		AudioPlayerP audio_player;
	private:
		float game_timer = 0.0f;
		bool is_game_timer_started = false;
		u32 enemy_count = 0;
	};
}

