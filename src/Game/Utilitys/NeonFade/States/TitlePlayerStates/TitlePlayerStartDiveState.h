#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"

namespace NeonFade {
	class TitlePlayer;
	class TitlePlayerStartDiveState :
		public IState
	{
	public:
		TitlePlayerStartDiveState(TitlePlayer* owner);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void DebugDraw() override;

	private:
		TitlePlayer* owner_player = nullptr;
		Animator* anim = nullptr;
		ModelRenderer* model = nullptr;
		Vector3 initial_position; // ダイブ開始時の位置
		Quaternion initial_rotation; // ダイブ開始時の回転
		Quaternion diving_rotation; // ダイブ後の回転
		float dive_timer = 0.0f;
		static constexpr float DIVE_DURATION = 0.5f; // ダイブアニメーションの再生時間（秒）
		static constexpr float DIVE_SPEED = 0.2f; // ダイブ中の移動速度（パスに沿って移動する速さ）
		static constexpr float MINIMUM_DIVE_TIME = 2.0f; // ダイブ中の最小時間（秒）
		std::unique_ptr<CatmullRomPath> path; // カメラの移動パス
		GameObjectWP camera_obj = nullptr;
		bool next = false;
	};
}

