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
		owner_scene_game->text_comp->SetText(u8"Ten State");
		for (u8 i = 0; i < 10; i++) {
			auto enem = SceneManager::Object::Create<Enemy>(u8"敵");
			enem->transform->scale = { 0.05f,0.05f,0.05f };
			enem->transform->position = { (float)i,60,180 };
			owner_scene_game->AddEnemyCount();
		}
	}
	void SceneGameState_Ten::OnExit(ISceneStateMachine* machine)
	{
	}
	void SceneGameState_Ten::Update(ISceneStateMachine* machine, float dt)
	{
	}
}