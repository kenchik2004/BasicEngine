//---------------------------------------------------------------------------
//! @file   PlayerSmashMainState.h
//! @brief  プレイヤーのスマッシュメイン状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class PlayerSmashMainState :
		public IState
	{
	public:
		PlayerSmashMainState(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void DebugDraw() override;
		float smash_timer = 0.0f;
	private:
		static constexpr float MAX_SMASH_TIME = 1.5f;
		static constexpr float CAMERA_SHAKE_TIME = 0.5f;
		static constexpr float SMASH_VELOCITY_FORWARD = 120.0f;
		static constexpr float SMASH_VELOCITY_DOWN = 20.0f;
		static constexpr int SMASH_DAMAGE = 60;
		static inline const Vector3 CAMERA_SHAKE_POWER = { 0.5f, 0.5f, 0.5f };

		Player* owner_player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		bool anim_after_smash = false;
		SafeSharedPtr<Collider> hit_box = nullptr;
		SafeSharedPtr<AudioClip> smash_se;

	};
}

