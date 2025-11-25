#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyAttackMainState :
		public IState
	{
	public:
		EnemyAttackMainState(Enemy* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		float attack_timer = 0;
		static constexpr float ATTACK_TIME = 1.3f;
	private:
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		SafeSharedPtr<Collider> hit_box = nullptr;
	};
}

