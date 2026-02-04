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
		u32 teams = 3, enem_per_tems = 5;

		factory.MakeEnemyTeam(teams, enem_per_tems, owner_scene_game->player);
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