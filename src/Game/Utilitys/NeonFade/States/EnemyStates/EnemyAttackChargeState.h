#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {
	class Enemy;
	class EnemyAttackChargeState :
		public IState
	{
	public:
		EnemyAttackChargeState(Enemy* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		float charge_timer = 0;
		static constexpr float CHARGE_TIME = 1.0f;
	private:
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
	};
}

