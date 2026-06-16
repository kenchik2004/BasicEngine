#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyIdleState :
		public IState
	{
	public:
		EnemyIdleState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;

	private:
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
	};
}
