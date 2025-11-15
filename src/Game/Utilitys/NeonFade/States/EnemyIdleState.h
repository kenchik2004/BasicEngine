#pragma once
#include "IState.h"
namespace NeonFade {

	class Enemy;
	class EnemyIdleState :
		public IState
	{
	public:
		EnemyIdleState(Enemy* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {}
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}

	private:
		Enemy* enemy = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
	};

}
