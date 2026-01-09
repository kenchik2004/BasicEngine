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
		animator->anim_speed = 0.0001f;
		hit_stop_timer += Time::UnscaledDeltaTime();
		};
	std::function collision_rotate = [this]() {
		auto col = enemy->GetComponent<CapsuleCollider>();
		if (!col)
			return;
		col->rotation = Quaternion(DEG2RAD(90), { 0,1,0 });
		col->position = { 5.4f, 1.5f, 0 };
		col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon);
		};
	animator->SetAnimationCallBack("enemy_down", hit_stop, 10, "hit_stop");
	animator->SetAnimationCallBack("enemy_down", collision_rotate, 30, "collision_rotate");
	std::function default_change = [this]() {
		return exit_timer >= EXIT_TIME;
		};
	RegisterChangeRequest("idle", default_change, 1);
}

void NeonFade::EnemyDownState::OnEnter(IStateMachine* machine)
{
	animator->Play("enemy_down", false, 0.05f);
	hit_stop_timer = 0;
	exit_timer = 0;
	auto enem_machine = static_cast<EnemyStateMachine*>(machine);
	knock_back_vec = enem_machine->move_vec;
	if (knock_back_vec.isZero())
		knock_back_vec = -enemy->transform->AxisZ();
	Vector3 set_forward = -knock_back_vec;
	set_forward.y = 0;
	set_forward.normalize();
	enemy->transform->SetAxisZ(set_forward);
}

void NeonFade::EnemyDownState::OnExit(IStateMachine* machine)
{
	animator->anim_speed = 1.0f;
	auto col = enemy->GetComponent<CapsuleCollider>();
	col->position = { 5.4f,0,0 };
	col->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
	col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon | Collider::Layer::Player);

}

void NeonFade::EnemyDownState::Update(IStateMachine* machine, float dt)
{
	exit_timer += dt;
	if (hit_stop_timer > 0)
		hit_stop_timer += dt;
	if (hit_stop_timer > HITSTOP_TIME) {
		auto enem_machine = static_cast<EnemyStateMachine*>(machine);
		animator->anim_speed = 1.0f;
		rb->SetVelocity(knock_back_vec);
		hit_stop_timer = 0;
	}
}
