#include "SceneGameState_Ten.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	SceneGameState_Ten::SceneGameState_Ten(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
	}
	void SceneGameState_Ten::OnEnter(ISceneStateMachine* machine)
	{
		exit_timer = 0;
		EnemyFactory factory;
		factory.SetSpawnPosition({ 0,5,200 });
		factory.SetSpawnRadius(20);
		u32 teams = 12, enem_per_tems = 12;
		factory.MakeEnemyTeam(teams, enem_per_tems, owner_scene_game->player);
		owner_scene_game->AddEnemyCount(teams * enem_per_tems);

		owner_scene_game->text_comp->SetText(u8"Ten State");

	}
	void SceneGameState_Ten::OnExit(ISceneStateMachine* machine)
	{
	}
	void SceneGameState_Ten::Update(ISceneStateMachine* machine, float dt)
	{
	}
}