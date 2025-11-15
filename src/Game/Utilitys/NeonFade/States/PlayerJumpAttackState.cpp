#include "PlayerJumpAttackState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	PlayerJumpAttackState::PlayerJumpAttackState(Player* player_)
		:IState(static_cast<GameObject*>(player_))
	{
		owner_player = player_;
		rb = player_->rb.lock().get();
		animator = player_->animator.lock().get();
		std::function exit_request = [this]() {
			return exit_timer > exit_time;
			};
		RegisterChangeRequest("jump_attack_land", exit_request, 0);
	}
	void PlayerJumpAttackState::OnEnter(IStateMachine* machine)
	{
		exit_timer = 0.0f;
		rb->use_gravity = false;
		rb->velocity.y = 0;
		atk_main_started = false;
		{
			atk_range_trigger = owner_player->AddComponent<SphereCollider>();
			atk_range_trigger->SetLayer(Collider::Layer::Wepon);
			atk_range_trigger->SetHitGroup(Collider::Layer::Enemy);
			atk_range_trigger->radius = 25.0f;
			atk_range_trigger->is_trigger = true;
		}
	}
	void PlayerJumpAttackState::OnExit(IStateMachine* machine)
	{
		rb->is_kinematic = false;
		rb->use_gravity = true;
		if (target) {
			Vector3 knock_vec = target->transform->position - owner_player->transform->position;
			knock_vec = ProjectOnPlane(knock_vec, { 0,1,0 });
			target->Down(knock_vec);
			target->rb->is_kinematic = false;
		}
		target = nullptr;
		atk_range_trigger->RemoveThisComponent();
	}
	void PlayerJumpAttackState::Update(IStateMachine* machine, float dt)
	{
		exit_timer += dt;

		if (exit_timer <= 0.3f && target) {
			Vector3 target_vec = target->transform->position - owner_player->transform->position;
			if (target_vec.magnitudeSquared() >= (7.5f * 7.5f))
				target_vec = target_vec.getNormalized() * 50;
			else {
				exit_timer = 0.3f;
				target_vec = { 0,0,0 };
			}
			rb->velocity = target_vec;
			target_vec = ProjectOnPlane(target_vec, { 0,1,0 });
			if (target_vec.magnitudeSquared() >= 1e-6f * 1e-6f)
				owner_player->transform->SetAxisZ(-target_vec);
		}
		if (exit_timer > 0.3f && !atk_main_started) {
			atk_main_started = true;

			rb->SetVelocity({ 0,0,0 });
			rb->is_kinematic = true;
			if (target) {
				animator->PlayIfNoSame("jump_attack", true, 0.0f, 0.02f, true);
				target->rb->SetVelocity({ 0,0,0 });
				target->rb->is_kinematic = true;

			}
			else {
				machine->ChangeState("idle");
			}
		}
		else if (exit_timer > 0.3f) {
			int frame = static_cast<int>(exit_timer * 100.0f);
			if (frame % 8 == 0)
				target->Damage(1);
		}

	}
	void PlayerJumpAttackState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == atk_range_trigger) {
			if (!target) {
				target = static_cast<Enemy*>(hit_info.hit_collision->owner.lock().get());
			}
		}
	}
	void PlayerJumpAttackState::DebugDraw()
	{

		int frame = static_cast<int>(exit_timer * 100.0f);
		if (frame % 3 == 0)
			printfDx("Atk!!\n");

	}
}
