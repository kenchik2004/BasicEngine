//---------------------------------------------------------------------------
//! @file   SceneGameState_KI.h
//! @brief  ゲームの起（序盤）シーン状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"

class CatmullRomPath;
class CameraObject;
namespace NeonFade {

	//----------------------------------------
	// 起承転結の「起」
	// ほかの名前が思いつかなかった。許せ。
	//----------------------------------------
	class MovieCameraMachine;
	class SceneGame;
	class SceneGameState_KI :
		public ISceneState
	{
	public:
		SceneGameState_KI(SceneGame* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) override;
		virtual void OnExit(ISceneStateMachine* machine) override;
		virtual void Update(ISceneStateMachine* machine, float dt) override;

		void DebugDraw() override;
	private:
		SceneGame* owner_scene_game = nullptr;
		float exit_timer = 0;
		static constexpr float EXIT_TIME = 10.0f;
		SafeWeakPtr<MovieCameraMachine> movie_camera = nullptr;
		SafeWeakPtr<CameraObject> scene_camera = nullptr;

		SafeUniquePtr<CatmullRomPath> camera_path = nullptr;
		TextWP message_text = nullptr;
		UIObjectWP timer_text = nullptr;
	};
}

