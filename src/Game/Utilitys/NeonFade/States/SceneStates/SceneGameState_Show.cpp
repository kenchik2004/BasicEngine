#include "SceneGameState_Show.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade
{
	SceneGameState_Show::SceneGameState_Show(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
	}
	void SceneGameState_Show::OnEnter(ISceneStateMachine* machine)
	{
		EnemyFactory factory;
		factory.SetSpawnPosition({ 0,5,200 });
		factory.SetSpawnRadius(20);
		u32 teams = 20, enem_per_tems = 10;
		for (u32 i = 0; i < teams; i++) {
			factory.MakeEnemyTeam(1, enem_per_tems, owner_scene_game->player);
			factory.SetSpawnPosition(Random::Position({ -30,5,180 }, { 30,5,400 }));

		}
		owner_scene_game->AddEnemyCount(teams * enem_per_tems);
		owner_scene_game->text_comp->SetText(u8"Show State");
	}
	void SceneGameState_Show::OnExit(ISceneStateMachine* machine)
	{
	}
	void SceneGameState_Show::Update(ISceneStateMachine* machine, float dt)
	{
	}

}