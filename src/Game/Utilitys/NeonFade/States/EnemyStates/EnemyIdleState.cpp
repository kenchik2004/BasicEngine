#include "precompile.h"
#include "EnemyIdleState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

namespace NeonFade
{

	EnemyIdleState::EnemyIdleState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
	}

	void EnemyIdleState::OnEnter(IStateMachine* machine)
	{
		animator->PlayIfNoSame("enemy_idle", true);
	}

	void EnemyIdleState::OnExit(IStateMachine* machine)
	{}

	void EnemyIdleState::Update(IStateMachine* machine, float dt)
	{
		//ちょっと密集しすぎると重なって見えるので、RVOを使って少し離すようにする
		Vector3 cohesion_force = {0.0f, 0.0f, 0.0f};
		EnemyRVOSystem::CalculateCohesion(cohesion_force, owner_enemy->transform.get(), owner_enemy, 2.0f);
		EnemyRVOSystem::ApplyMovement(cohesion_force, rb, 1.0f);

	}



}