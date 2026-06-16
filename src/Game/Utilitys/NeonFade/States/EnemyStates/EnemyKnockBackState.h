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


	private:
		void RotateCollider();
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		CapsuleCollider* col = nullptr;
		static constexpr float KNOCK_BACK_DURATION = 2.0f;
		float elapsed_time = 0.0f;

	
	};
}

