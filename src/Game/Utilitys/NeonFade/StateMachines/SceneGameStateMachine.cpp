//---------------------------------------------------------------------------
//! @file   SceneGameStateMachine.cpp
//! @brief  SceneGameStateMachineの実装。ゲームシーンのステートマシン処理を行う
//---------------------------------------------------------------------------
#include "SceneGameStateMachine.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_KI.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Show.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Ten.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Ketsu.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Clear.h"
#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Over.h"

namespace NeonFade {
	SceneGameStateMachine::SceneGameStateMachine(SceneGame* owner_scene_)
		:ISceneStateMachine(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
		auto ki_state = make_safe_unique<SceneGameState_KI>(owner_scene_game);
		auto show_state = make_safe_unique<SceneGameState_Show>(owner_scene_game);
		auto ten_state = make_safe_unique<SceneGameState_Ten>(owner_scene_game);
		auto ketsu_state = make_safe_unique<SceneGameState_Ketsu>(owner_scene_game);
		auto clear_state = make_safe_unique<SceneGameState_Clear>(owner_scene_game);
		auto over_state = make_safe_unique<SceneGameState_Over>(owner_scene_game);

		std::function<bool()> ki_to_over = [this]() {
			return owner_scene_game->GetGameTimer() > SceneGame::GAME_TIMER_MAX;
			};
		ki_state->RegisterChangeRequest("Over", ki_to_over, 0);

		AddState("KI", std::move(ki_state));

		std::function<bool()> show_to_ten = [this]() {
			return
				owner_scene_game->GetEnemyCount() == 0 &&
				!owner_scene_game->IsEffectExsist() &&
				!owner_scene_game->IsEffectPreparing();
			};
		std::function<bool()> show_to_over = [this]() {
			return owner_scene_game->GetGameTimer() > SceneGame::GAME_TIMER_MAX;
			};
		show_state->RegisterChangeRequest("Ten", show_to_ten, 0);
		show_state->RegisterChangeRequest("Over", show_to_over, 0);
		AddState("Show", std::move(show_state));

		std::function<bool()> ten_to_over = [this]() {
			return owner_scene_game->GetGameTimer() > SceneGame::GAME_TIMER_MAX;
			};

		ten_state->RegisterChangeRequest("Over", ten_to_over, 0);
		AddState("Ten", std::move(ten_state));


		std::function<bool()> ketsu_to_fin = [this]() {
			return
				owner_scene_game->GetEnemyCount() == 0 &&
				!owner_scene_game->IsEffectExsist() &&
				!owner_scene_game->IsEffectPreparing();
			};
		std::function<bool()> ketsu_to_over = [this]() {
			return owner_scene_game->GetGameTimer() > SceneGame::GAME_TIMER_MAX;
			};
		ketsu_state->RegisterChangeRequest("Clear", ketsu_to_fin, 0);
		ketsu_state->RegisterChangeRequest("Over", ketsu_to_over, 0);
		AddState("Ketsu", std::move(ketsu_state));
		AddState("Clear", std::move(clear_state));
		AddState("Over", std::move(over_state));

		ChangeState("KI");

	}
}