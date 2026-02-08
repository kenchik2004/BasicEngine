#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyLeaderStepBackState :
		public IState
	{
	public:
		EnemyLeaderStepBackState(Enemy* owner_, GameObjectWP player_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
	public:
		float step_back_timer = 0.0f;
		const float STEP_BACK_TIME = 2.0f;
	private:
		Enemy* enemy;
		RigidBody* rb;
		Animator* animator;
		GameObjectWP player;
		bool jumped = false;
	};
}

