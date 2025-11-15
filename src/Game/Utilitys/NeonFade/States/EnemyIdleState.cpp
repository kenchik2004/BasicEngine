#include "precompile.h"
#include "EnemyIdleState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

NeonFade::EnemyIdleState::EnemyIdleState(Enemy* owner_) :
	IState(static_cast<GameObject*>(owner_))
{
	enemy = owner_;
	rb = owner_->rb.lock().get();
	animator = owner_->animator.lock().get();

}

void NeonFade::EnemyIdleState::OnEnter(IStateMachine* machine)
{
	animator->Play("enemy_idle");
}

void NeonFade::EnemyIdleState::OnExit(IStateMachine* machine)
{
}

void NeonFade::EnemyIdleState::Update(IStateMachine* machine, float dt)
{
}
