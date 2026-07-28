//---------------------------------------------------------------------------
//! @file   SceneGameState_Show.cpp
//! @brief  SceneGameState_Showの実装。ゲームの演出シーン状態処理を行う
//---------------------------------------------------------------------------
#include "SceneGameState_Show.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"

namespace NeonFade
{
	SceneGameState_Show::SceneGameState_Show(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;

		timer_text = owner_scene_game->ui_texts["txt_time"];
	}
	void SceneGameState_Show::OnEnter(ISceneStateMachine* machine)
	{
		EnemyFactory factory;
		factory.SetSpawnPosition({ 0,5,200 });
		factory.SetSpawnRadius(20);
		u32 teams = FileSystem::IniFileManager::GetInt("Game", "team_counts", 1, "data/config.ini");
		u32	enemys_per_team = FileSystem::IniFileManager::GetInt("Game", "members_per_team", 5, "data/config.ini");
		bool is_team_mode = FileSystem::IniFileManager::GetBool("Game", "teamed_enemy", false, "data/config.ini");
		{
			if (is_team_mode) {

				enemy_teams = factory.MakeEnemyTeam(teams, enemys_per_team, owner_scene_game->player);
			}
			else {

				for (u32 i = 0; i < teams * enemys_per_team; ++i)
				{
					factory.MakeBasicEnemy(owner_scene_game->player);
				}
			}

			Time::ResetTime();
		}
		owner_scene_game->AddEnemyCount(teams * enemys_per_team);
		owner_scene_game->text_comp->SetText(u8"Show State");
		owner_scene_game->ResetGameTimer();
		owner_scene_game->StartGameTimer();
		owner_scene_game->SetPauseAvailable(true);
		timer_text->WakeUp();
	}
	void SceneGameState_Show::OnExit(ISceneStateMachine* machine)
	{
		enemy_teams.clear();
	}
	void SceneGameState_Show::Update(ISceneStateMachine* machine, float dt)
	{

	}

}