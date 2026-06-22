//---------------------------------------------------------------------------
//! @file   PlayerCombatComboState.h
//! @brief  プレイヤーの連続攻撃コンボ状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Player;
	class Enemy;
	class PlayerCombatComboState :
		public IState
	{
	public:
		PlayerCombatComboState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void DebugDraw() override;
	private:
		Player* player = nullptr;
		SafeWeakPtr<Enemy> target = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		SafeWeakPtr<Collider> hit_box = nullptr;
		bool input_limit = false;
		bool next_avalable = false;
		bool knock_back = false;

		float attack_timer = 0.0f;
		static constexpr float ATTACK_TIME = 0.65f;
		float hit_stop_timer = 0.0f;
		static constexpr float HIT_STOP_TIME = 0.1f;
		static constexpr float CAMERA_SHAKE_TIME = 0.2f;
		static constexpr float CAMERA_SHAKE_POWER = 0.2f;
		static constexpr u32 DAMAGE = 10;

	};

}
