#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyMoveState :
		public IState
	{
	public:
		EnemyMoveState(Enemy* enemy_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
	private:
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
	};

}
