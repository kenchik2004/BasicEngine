//---------------------------------------------------------------------------
//! @file   SceneGameState_Over.h
//! @brief  ゲームオーバー時のシーン状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"
namespace NeonFade {
	//----------------------------------------
	// ゲームオーバーのシーン
	//----------------------------------------
	class SceneGame;
	class SceneGameState_Over :
		public ISceneState
	{
	public:
		SceneGameState_Over(SceneGame* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) override;
		virtual void OnExit(ISceneStateMachine* machine) override;
		virtual void Update(ISceneStateMachine* machine, float dt) override;
	private:
		SceneGame* owner_scene_game = nullptr;
		bool is_exit_requested = false;
	};
}

