#include "SceneGameState_Ketsu.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyTeam.h"

namespace NeonFade {
	SceneGameState_Ketsu::SceneGameState_Ketsu(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
	}
	void SceneGameState_Ketsu::OnEnter(ISceneStateMachine* machine)
	{

		EnemyFactory factory;
		factory.SetSpawnPosition({ 0,5,200 });
		factory.SetSpawnRadius(20);
		u32 teams = 8, enem_per_tems = 8;
		enemy_teams = factory.MakeEnemyTeam(teams, enem_per_tems, owner_scene_game->player);
		owner_scene_game->AddEnemyCount(teams * enem_per_tems);

		owner_scene_game->text_comp->SetText(u8"KETSU State");
	}
	void SceneGameState_Ketsu::OnExit(ISceneStateMachine* machine)
	{
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