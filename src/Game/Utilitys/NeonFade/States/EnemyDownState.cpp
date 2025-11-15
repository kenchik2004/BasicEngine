#include "precompile.h"
#include "EnemyDownState.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"

NeonFade::EnemyDownState::EnemyDownState(Enemy* owner_)
	:IState(static_cast<GameObject*>(owner_))
{
	enemy = owner_;
	animator = enemy->animator.lock().get();
	rb = enemy->rb.lock().get();

	std::function hit_stop = [this]() {
		animator->anim_speed = 0.001f;
		hit_stop_timer += Time::UnscaledDeltaTime();
		};
	std::function collision_rotate = [this]() {
		auto col = enemy->GetComponent<CapsuleCollider>();
		col->rotation = Quaternion(DEG2RAD(-90), { 0,1,0 });
		col->position = {5.4f, 1.5f, 0};
		};
	animator->SetAnimationCallBack("enemy_down", hit_stop, 7, "hit_stop");
	animator->SetAnimationCallBack("enemy_down", collision_rotate, 30, "collision_rotate");
}

void NeonFade::EnemyDownState::OnEnter(IStateMachine* machine)
{
	animator->Play("enemy_down", false, 0.05f);
	hit_stop_timer = 0;
}

void NeonFade::EnemyDownState::OnExit(IStateMachine* machine)
{
	animator->anim_speed = 1.0f;
	auto col = enemy->GetComponent<CapsuleCollider>();
	col->position = { 5.4f,0,0 };
	col->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
}

void NeonFade::EnemyDownState::Update(IStateMachine* machine, float dt)
{
	if (!animator->IsPlaying()) {

		machine->ChangeState("idle");
	}
	if (hit_stop_timer > 0)
		hit_stop_timer += Time::UnscaledDeltaTime();
	if (hit_stop_timer > 0.1f) {
		animator->anim_speed = 1.0f;
		rb->SetVelocity(enemy->transform->AxisZ() * 20 + Vector3(0, 20, 0));
		hit_stop_timer = 0;
	}
}
