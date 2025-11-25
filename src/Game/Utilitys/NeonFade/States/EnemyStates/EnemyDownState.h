#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {

	class Enemy;
	class EnemyDownState :
		public IState
	{
	public:
		EnemyDownState(Enemy* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {}
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		float exit_timer = 0;
		static constexpr float EXIT_TIME = 0.7f;
	private:
		Enemy* enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb;
		float hit_stop_timer = 0;
	};
}

