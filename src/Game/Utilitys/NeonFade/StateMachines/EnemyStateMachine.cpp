#include "precompile.h"
#include "EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	EnemyStateMachine::EnemyStateMachine(Enemy* owner_)
		:IStateMachine(static_cast<GameObject*>(owner_))
	{
		enemy = owner_;
	}
	EnemyStateMachine::~EnemyStateMachine()
	{
	}
}
