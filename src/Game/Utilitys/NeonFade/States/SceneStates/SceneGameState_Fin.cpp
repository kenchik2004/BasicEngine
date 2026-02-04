#include "Game/Utilitys/NeonFade/States/SceneStates/SceneGameState_Fin.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {
	SceneGameState_Fin::SceneGameState_Fin(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
		std::function<bool()> fin_to_exit = [this]() {
			return exit_timer > EXIT_TIME;
			};
		RegisterChangeRequest("KI", fin_to_exit, 0);
	}
	void SceneGameState_Fin::OnEnter(ISceneStateMachine* machine)
	{
		exit_timer = 0;
		owner_scene_game->text_comp->SetText(u8"Fin State");
	}
	void SceneGameState_Fin::OnExit(ISceneStateMachine* machine)
	{
		SceneManager::CloseApplication();
	}
	void SceneGameState_Fin::Update(ISceneStateMachine* machine, float dt)
	{
		exit_timer += dt;
	}
}
