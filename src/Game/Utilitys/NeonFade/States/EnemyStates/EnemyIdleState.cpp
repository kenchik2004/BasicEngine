#include "precompile.h"
#include "EnemyIdleState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade
{

	EnemyIdleState::EnemyIdleState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
	}

	void EnemyIdleState::OnEnter(IStateMachine* machine)
	{
		animator->PlayIfNoSame("enemy_idle", true);
	}

	void EnemyIdleState::OnExit(IStateMachine* machine)
	{}

	void EnemyIdleState::Update(IStateMachine* machine, float dt)
	{}



}