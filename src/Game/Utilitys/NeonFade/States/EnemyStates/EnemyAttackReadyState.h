#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {

	class Enemy;
	class EnemyAttackReadyState :
		public IState
	{
	public:
		EnemyAttackReadyState(Enemy* enemy_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
	private:
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		GameObjectWP target = nullptr;
		bool next = false;
		static constexpr float max_ready_time = 4.0f;
		float ready_timer = 0.0f;
		float random_attack_distance = 0.0f;
	};

}
