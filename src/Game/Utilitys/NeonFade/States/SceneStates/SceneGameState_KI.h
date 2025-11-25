#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"
namespace NeonFade {

	//----------------------------------------
	// 起承転結の「起」
	// ほかの名前が思いつかなかった。許せ。
	//----------------------------------------
	class SceneGame;
	class SceneGameState_KI :
		public ISceneState
	{
	public:
		SceneGameState_KI(SceneGame* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) override;
		virtual void OnExit(ISceneStateMachine* machine) override;
		virtual void Update(ISceneStateMachine* machine, float dt) override;
	private:
		SceneGame* owner_scene_game = nullptr;
		float exit_timer = 0;
		static constexpr float EXIT_TIME = 2.0f;
	};
}

