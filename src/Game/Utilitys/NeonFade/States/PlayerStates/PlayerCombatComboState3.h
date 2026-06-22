//---------------------------------------------------------------------------
//! @file   PlayerCombatComboState3.h
//! @brief  プレイヤーの連続攻撃コンボ第3段状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {
	class Player;
	class PlayerCombatComboState3 :
		public IState
	{
	public:
		PlayerCombatComboState3(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
	private:
		Player* player = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		bool input_limit = false;
		bool next_avalable = false;
		bool knock_back = false;
		SafeWeakPtr<SphereCollider> hit_box;
		SafeWeakPtr<class Enemy> target;
		float attack_timer = 0.0f;
		static constexpr float ATTACK_TIME = 0.45f;
		float hit_stop_timer = 0.0f;
		static constexpr float HIT_STOP_TIME = 0.15f;
		static constexpr float CAMERA_SHAKE_TIME = 0.3f;
		static constexpr float CAMERA_SHAKE_POWER = 0.5f;
		static constexpr u32 DAMAGE = 30;
	};
}

