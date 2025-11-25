#pragma once
#include "IStateMachine.h"
namespace NeonFade {
	class Enemy;
	class EnemyStateMachine :
		public IStateMachine
	{
	public:
		EnemyStateMachine(Enemy* owner_);
		void DebugDraw() override;
		void OnTriggerEnter(const HitInfo& hit_info) override;
		virtual ~EnemyStateMachine();
		Enemy* enemy = nullptr;
		bool is_damaged = false;
		bool knock_back = false;
		bool is_dead = false;
		bool is_attacking = false;
		Vector3 move_vec = { 0,0,0 };
	private:
	};
}

