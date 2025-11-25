#include "SceneGameState_Ketsu.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {
	SceneGameState_Ketsu::SceneGameState_Ketsu(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
		//仮シーケンスのため、「起」へ戻る条件を登録
		std::function<bool()> ketsu_to_ki = [this]() {
			return exit_timer > EXIT_TIME;
			};
		RegisterChangeRequest("KI", ketsu_to_ki, 0);
	}
	void SceneGameState_Ketsu::OnEnter(ISceneStateMachine* machine)
	{
		owner_scene_game->text_comp->SetText(u8"KETSU State");
		exit_timer = 0;
	}
	void SceneGameState_Ketsu::OnExit(ISceneStateMachine* machine)
	{
	}
	void SceneGameState_Ketsu::Update(ISceneStateMachine* machine, float dt)
	{
		exit_timer += dt;
	}
}