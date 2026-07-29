//---------------------------------------------------------------------------
//! @file   SceneGameState_Ketsu.cpp
//! @brief  SceneGameState_Ketsuの実装。ゲームの結（終盤）シーン状態処理を行う
//---------------------------------------------------------------------------
#include "SceneGameState_Ketsu.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"

namespace NeonFade {
	SceneGameState_Ketsu::SceneGameState_Ketsu(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;

		timer_text = owner_scene_game->ui_texts["txt_time"]->GetComponent<Text>();
	}
	void SceneGameState_Ketsu::OnEnter(ISceneStateMachine* machine)
	{

		EnemyFactory factory;
		factory.SetSpawnPosition({ 0,5,200 });
		factory.SetSpawnRadius(20);
		u32 teams = FileSystem::IniFileManager::GetInt("Game", "team_counts", 1, "data/config.ini");
		u32	enemys_per_team = FileSystem::IniFileManager::GetInt("Game", "members_per_team", 5, "data/config.ini");
		bool is_team_mode = FileSystem::IniFileManager::GetBool("Game", "teamed_enemy", false, "data/config.ini");
		{
			if (is_team_mode) {

				enemy_teams = factory.MakeEnemyTeams(teams, enemys_per_team, owner_scene_game->player);
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

		owner_scene_game->text_comp->SetText(u8"KETSU State");
		}
	void SceneGameState_Ketsu::OnExit(ISceneStateMachine* machine)
	{
		owner_scene_game->StopGameTimer();
		timer_text->Sleep();
	}
	void SceneGameState_Ketsu::Update(ISceneStateMachine* machine, float dt)
	{
		for (auto team = enemy_teams.begin(); team != enemy_teams.end();)
		{
			if ((*team)->GetMemberNum() < 3)
			{
				(*team)->ClearTeam();
				team = enemy_teams.erase(team);
				continue;
			}
			team++;

		}
	}
	}