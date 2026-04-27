//---------------------------------------------------------------------------
//! @file   SceneGameState_Clear.h
//! @brief  ゲームクリア時のシーン状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"


namespace NeonFade {
	//----------------------------------------
	// 起承転結の後に来る、Exitシーン
	//----------------------------------------
	class SceneGame;
	class SceneGameState_Clear :
		public ISceneState
	{
	public:
		SceneGameState_Clear(SceneGame* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) override;
		virtual void OnExit(ISceneStateMachine* machine) override;
		virtual void Update(ISceneStateMachine* machine, float dt) override;
	private:
		SceneGame* owner_scene_game = nullptr;
		float exit_timer = 0;
		static constexpr float EXIT_TIME = 5.0f;
		UIObjectWP fin_text_obj;
		SafeSharedPtr<AudioClip> fin_se;
	};
}

