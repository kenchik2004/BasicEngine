#include "EnemyMoveState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

namespace NeonFade
{
	EnemyMoveState::EnemyMoveState(Enemy* enemy_)
		:IState(static_cast<GameObject*>(enemy_))
	{
		owner_enemy = enemy_;
		rb = enemy_->rb.lock().get();
		animator = enemy_->animator.lock().get();
	}
	void EnemyMoveState::OnEnter(IStateMachine* machine)
	{
		animator->Play("enemy_walk", true, 0, 0.1f);
	}
	void EnemyMoveState::Update(IStateMachine* machine, float dt)
	{
		//進行方向に向ける
		auto enem_machine = static_cast<EnemyStateMachine*>(machine);
		float dot = owner_enemy->transform->AxisZ().dot(enem_machine->move_vec);
		if (fabsf(dot) > FLT_EPSILON)
			owner_enemy->transform->SetAxisZ(enem_machine->move_vec.getNormalized());
		//移動
		float vel_y = rb->velocity.y;
		rb->velocity = enem_machine->move_vec;
		rb->velocity.y = vel_y;

	}
	void EnemyMoveState::OnExit(IStateMachine* machine)
	{
	}
}