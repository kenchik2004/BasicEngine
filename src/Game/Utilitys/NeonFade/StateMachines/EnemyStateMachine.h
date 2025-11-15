#pragma once
#include "IStateMachine.h"
namespace NeonFade {
	class Enemy;
	class EnemyStateMachine :
		public IStateMachine
	{
	public:
		EnemyStateMachine(Enemy* owner_);
		virtual ~EnemyStateMachine();
	private:
		Enemy* enemy = nullptr;
	};
}

