#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {

	class Enemy;
	class EnemyDamageState :
		public IState
	{
	public:

		EnemyDamageState(Enemy* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {}
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		static constexpr float HITSTOP_TIME = 0.2f;
	private:
		Enemy* enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb;
		float hit_stop_timer = 0;
	};

}
