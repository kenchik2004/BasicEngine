#include "precompile.h"
#include "EnemyDamageState.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"

NeonFade::EnemyDamageState::EnemyDamageState(Enemy* owner_) :
	IState(static_cast<GameObject*>(owner_))
{

	enemy = owner_;
	animator = enemy->animator.lock().get();
	rb = enemy->rb.lock().get();
	std::function hit_stop = [this]() {
		animator->anim_speed = 0.001f;
		hit_stop_timer += Time::UnscaledDeltaTime();
		};
	animator->SetAnimationCallBack("enemy_damage", hit_stop, 3, "hit_stop");
	std::function default_change = [this]() {
		return !animator->IsPlaying();
		};
	RegisterChangeRequest("idle", default_change, 1);
}

void NeonFade::EnemyDamageState::OnEnter(IStateMachine* machine)
{
	animator->Play("enemy_damage");
	animator->anim_speed = 1.6f;
	hit_stop_timer = 0;
	//rb->velocity = { 0, 0, 0 };
}

void NeonFade::EnemyDamageState::OnExit(IStateMachine* machine)
{
	//animator->Stop();
	animator->anim_speed = 1.0f;
}

void NeonFade::EnemyDamageState::Update(IStateMachine* machine, float dt)
{
	if (!animator->IsPlaying())
		machine->ChangeState("idle");
	if (hit_stop_timer > 0)
		hit_stop_timer += Time::UnscaledDeltaTime();
	if (hit_stop_timer > HITSTOP_TIME) {
		animator->anim_speed = 1.6f;
		hit_stop_timer = 0;
	}

}
