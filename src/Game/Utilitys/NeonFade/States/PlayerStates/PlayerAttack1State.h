//---------------------------------------------------------------------------
//! @file   PlayerAttack1State.h
//! @brief  プレイヤーの第1攻撃状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class PlayerAttack1State :
		public IState
	{
	public:
		PlayerAttack1State(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)override;
		static constexpr float EXIT_TIME = 0.7f;
		static constexpr float EARLY_EXIT_TIME = 0.5f;	// 早期終了時間（ステートの早期終了を許可する時間）
		static constexpr float SPIN_TIME = 0.4f;
		float exit_timer = 0.0f;
		float hit_stop_timer = 0.0f;

		static constexpr float KNOCK_BACK_FORCE = 70.0f;
		static constexpr float KNOCK_BACK_DOWN_FORCE = -10.0f;
		static constexpr u32 DAMAGE = 20;

		static constexpr float HIT_STOP_TIME = 0.2f;
		static constexpr float CAMERA_SHAKE_TIME = 0.2f;
		static inline const Vector3 CAMERA_SHAKE_INTENSITY = { 1.5f, 1.5f, 1.5f };
	private:
		Player* owner_player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		SafeSharedPtr<Collider> hit_box = nullptr;

		Quaternion spin_rot = Quaternion(physx::PxIdentity);
		Quaternion start_rot = Quaternion(physx::PxIdentity);
		GameObjectWP eff = nullptr;
		bool early_exit = false;
	};
}

