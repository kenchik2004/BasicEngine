#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyKnockBackState :
		public IState
	{
	public:
		EnemyKnockBackState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		bool CanTransitTo(const std::string& state_name) override;

		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;


	private:
		void RotateCollider();
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		CapsuleCollider* col = nullptr;
		static constexpr float KNOCK_BACK_DURATION = 2.0f;
		static constexpr float CHAIN_KNOCK_BACK_START_TIME = 0.1f; // チェインノック(連鎖して当たった他の敵も吹き飛ばす)判定の開始時間
		static constexpr float CHAIN_KNOCK_BACK_DURATION = 0.4f; // チェインノック(連鎖して当たった他の敵も吹き飛ばす)判定の開時間

		ColliderWP chain_knock_collider = nullptr; // チェインノックバックの判定用コライダー
		static constexpr float CHAIN_KNOCK_BACK_FORCE = 20.0f; // チェインノックバックの力
		static constexpr float CHAIN_KNOCK_BACK_UP_FORCE = 10.0f; // チェインノックバックの上方向の力
		static constexpr u32 CHAIN_KNOCK_BACK_DAMAGE = 5; // チェインノックバックのダメージ

		float elapsed_time = 0.0f;


	};
}

