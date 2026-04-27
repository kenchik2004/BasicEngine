//---------------------------------------------------------------------------
//! @file   EnemyDownState.cpp
//! @brief  EnemyDownStateの実装。敵のダウン状態の処理を行う
//---------------------------------------------------------------------------
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


	if (!hit_se)
		hit_se = AudioManager::CloneByName(u8"hit_se");

	{
		std::function hit_stop = [this]() {
			animator->anim_speed = 0.0001f;
			hit_stop_timer += Time::UnscaledDeltaTime();
			};
		std::function collision_rotate = [this]() {
			auto col = enemy->GetComponent<CapsuleCollider>();
			if (!col)
				return;
			col->rotation = Quaternion(DEG2RAD(90), { 0,1,0 });
			col->position = { 0,1.0f,0 };
			col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon);
			if (knockout_se)
				knockout_se->PlayOneShot();
			};
		animator->SetAnimationCallBack("enemy_down", hit_stop, 10, "hit_stop");
		animator->SetAnimationCallBack("enemy_down", collision_rotate, 30, "collision_rotate");
	}
	{
		std::function hit_stop = [this]() {
			animator->anim_speed = 0.0001f;
			hit_stop_timer += Time::UnscaledDeltaTime();
			};
		std::function collision_rotate = [this]() {
			auto col = enemy->GetComponent<CapsuleCollider>();
			if (!col)
				return;
			col->rotation = Quaternion(DEG2RAD(90), { 0,1,0 });
			col->position = { 0,1.0f,0 };
			col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon);
			if (knockout_se)
				knockout_se->PlayOneShot();
			};
		animator->SetAnimationCallBack("enemy_down_forward", hit_stop, 10, "hit_stop");
		animator->SetAnimationCallBack("enemy_down_forward", collision_rotate, 30, "collision_rotate");
	}
	std::function default_change = [this]() {
		return exit_timer >= EXIT_TIME;
		};
	RegisterChangeRequest("idle", default_change, 1);
	if (!knockout_se)
		knockout_se = AudioManager::CloneByName(u8"knockout_se");
}

void NeonFade::EnemyDownState::OnEnter(IStateMachine* machine)
{
	hit_stop_timer = 0;
	exit_timer = 0;
	if (hit_se)
		hit_se->PlayOneShot();
	auto enem_machine = static_cast<EnemyStateMachine*>(machine);
	knock_back_vec = enem_machine->move_vec;
	knock_back_vec.y = 0;
	Vector3 current_z = enemy->transform->AxisZ();
	bool down_forward = false;
	if (!knock_back_vec.isZero() && current_z.dot(knock_back_vec) > 0.5f)
		down_forward = true;

	if (knock_back_vec.isZero())
		knock_back_vec = -current_z;
	Vector3 set_forward = knock_back_vec;
	set_forward.normalize();
	if (down_forward) {								// だいたい前方に倒れる場合	
		enemy->transform->SetAxisZ(set_forward);
		animator->Play("enemy_down_forward", false, 0.05f, 0.2f, true);
	}
	else {											//倒れる方向と進行方向があまりにも違う場合
		enemy->transform->SetAxisZ(-set_forward);
		animator->Play("enemy_down", false, 0.05f, 0.2f, true);
	}
}

void NeonFade::EnemyDownState::OnExit(IStateMachine* machine)
{
	animator->anim_speed = 1.0f;
	auto col = enemy->GetComponent<CapsuleCollider>();
	col->rotation = Quaternion(DEG2RAD(90), { 0,0,-1 });
	col->position = { 0,0,0 };
	col->SetHitGroup(Collider::Layer::Terrain | Collider::Layer::Vehicle | Collider::Layer::Wepon | Collider::Layer::Player | Collider::Layer::Enemy);

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
