#include "precompile.h"
#include "SceneGameState_KI.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Utilitys/NeonFade/StateMachines/SceneGameStateMachine.h"


namespace NeonFade {
	SceneGameState_KI::SceneGameState_KI(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
	}
	void SceneGameState_KI::OnEnter(ISceneStateMachine* machine)
	{
		owner_scene_game->text_comp->SetText(u8"KI State");
		exit_timer = 0;
	}
	void SceneGameState_KI::OnExit(ISceneStateMachine* machine)
	{
	}
	void SceneGameState_KI::Update(ISceneStateMachine* machine, float dt)
	{
		exit_timer += dt;
		if (exit_timer > EXIT_TIME) {
			machine->ChangeState("Show");
			return;
		}
	}
}
