//---------------------------------------------------------------------------
//! @file   SceneGameState_Ketsu.cpp
//! @brief  SceneGameState_Ketsuの実装。ゲームの結（終盤）シーン状態処理を行う
//---------------------------------------------------------------------------
#include "SceneGameState_Ketsu.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyTeam.h"

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
		u32 teams = 3, enem_per_tems = 5;
#if 0
		enemy_teams = factory.MakeEnemyTeam(teams, enem_per_tems, owner_scene_game->player);
#else
		for (u32 i = 0; i < teams * enem_per_tems; ++i)
		{
			factory.MakeBasicEnemy(owner_scene_game->player);
		}	
#endif
		owner_scene_game->AddEnemyCount(teams * enem_per_tems);

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