#include "precompile.h"
#include "PlayerCombatComboState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"


namespace NeonFade {

	PlayerCombatComboState::PlayerCombatComboState(Player* owner_) :
		IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		rb = player->rb.lock().get();
		animator = player->animator.lock().get();
		std::function slip_stop = [this]() {
			rb->velocity = { 0,rb->velocity.y,0 };
			};

		std::function left_punch84 = [this]() {
			auto col = player->AddComponent<SphereCollider>
				(Vector3(0, 5, -5), Quaternion(0, 0, 0, 1), 3.0f,
					true, Collider::Layer::Wepon, Collider::Layer::Enemy);
			hit_box = std::move(col);

			{
				Vector2 input = Input::GetPadLeftStick(0) * -1;

				if (input.magnitudeSquared() > FLT_EPSILON) {

					Vector3 mov(0, 0, 0);
					if (target)
					{
						mov = target->transform->position - player->transform->position;

					}
					else {

						mov -= player->player_camera->transform->AxisX() * input.x * 10.0f;
						mov -= player->player_camera->transform->AxisZ() * input.y * 10.0f;
					}
					mov = ProjectOnPlane(mov, { 0,1,0 });
					rb->SetVelocity(mov);
					player->transform->SetAxisZ(-mov.getNormalized());
				}
			}

			};

#if 0
		std::function left_punch160 = [this]() {
			if (hit_box)
				hit_box->RemoveThisComponent();

			auto col = player->AddComponent<SphereCollider>();
			col->SetHitGroup(Collider::Layer::Enemy);
			col->SetLayer(Collider::Layer::Wepon);
			col->is_trigger = true;
			col->radius = 4;
			col->position = { 0,5,-4 };
			hit_box = std::move(col);
			animator->anim_speed = 0.2f;
			Time::SetTimeScale(0.0);
			player->GetScene()->physics_timescale = 0.1f;

			{
				Vector2 input = Input::GetPadLeftStick(0) * -1;

				if (input.magnitudeSquared() > FLT_EPSILON) {

					Vector3 mov(0, 0, 0);
					if (target)
					{
						if (target->IsDead())
							target = nullptr;
						else
							mov = target->transform->position - player->transform->position;

					}
					else {

						mov -= player->player_camera->transform->AxisX() * input.x * 5.0f;
						mov -= player->player_camera->transform->AxisZ() * input.y * 5.0f;
					}
					mov = ProjectOnPlane(mov, { 0,1,0 });
					rb->AddForce(mov * 10, ForceMode::Impulse);
					player->transform->SetAxisZ(-mov.getNormalized());
				}
			}

			};
		std::function left_punch160_fin = [this]() {
			animator->anim_speed = 4.0f;
			Time::SetTimeScale(1.0);
			player->GetScene()->physics_timescale = 2.0f;

			};
		std::function left_kick210 = [this]() {
			if (hit_box)
				hit_box->RemoveThisComponent();
			knock_back = true;

			auto col = player->AddComponent<SphereCollider>();
			col->SetHitGroup(Collider::Layer::Enemy);
			col->SetLayer(Collider::Layer::Wepon);
			col->radius = 4;
			col->is_trigger = true;
			col->position = { 0,5,-4 };
			hit_box = std::move(col);
			animator->anim_speed = 0.2f;
			Time::SetTimeScale(0.0);
			{
				Vector2 input = Input::GetPadLeftStick(0) * -1;

				if (input.magnitudeSquared() > FLT_EPSILON) {

					Vector3 mov(0, 0, 0);
					if (target)
					{
						mov = target->transform->position - player->transform->position;
					}
					else {

						mov -= player->player_camera->transform->AxisX() * input.x * 5.0f;
						mov -= player->player_camera->transform->AxisZ() * input.y * 5.0f;
					}
					mov = ProjectOnPlane(mov, { 0,1,0 });
					rb->AddForce(mov.getNormalized() * 10, ForceMode::Impulse);
					player->transform->SetAxisZ(-mov.getNormalized());
				}
			}
			};
		std::function left_kick210_fin = [this]() {
			animator->anim_speed = 4.0f;
			Time::SetTimeScale(1.0);
			};
#endif
		animator->SetAnimationCallBack("combat_combo", slip_stop, 40, "slip_stop");
		animator->SetAnimationCallBack("combat_combo", left_punch84, 84, "first_punch");

#if 0
		animator->SetAnimationCallBack("combat_combo", left_punch160, 160, "second_punch");
		animator->SetAnimationCallBack("combat_combo", left_punch160_fin, 164, "second_punch_fin");
		animator->SetAnimationCallBack("combat_combo", slip_stop3, 166, "slip_stop3");

		animator->SetAnimationCallBack("combat_combo", left_kick210, 210, "last_kick");
		animator->SetAnimationCallBack("combat_combo", left_kick210_fin, 213, "last_kick_fin");
		animator->SetAnimationCallBack("combat_combo", slip_stop4, 215, "slip_stop4");
#endif
		std::function<bool()> to_next = [this]() {
			return next_avalable && input_limit;
			};
		std::function<bool()> to_idle = [this]() {
			return !next_avalable && input_limit;
			};
		RegisterChangeRequest("attack2", to_next, 0);
		RegisterChangeRequest("idle", to_idle, 1);
	}
	void PlayerCombatComboState::OnEnter(IStateMachine* machine)
	{

		input_limit = false;
		next_avalable = false;
		knock_back = false;
		//rb->velocity = { rb->velocity.x * 0.5f, rb->velocity.y,rb->velocity.z * 0.5f };
		animator->Play("combat_combo", false, 0.9f, 0.1f);
		animator->anim_speed = 2.0f;
		hit_stop_timer = 0.0f;
		attack_timer = 0.0f;
		{
			Vector2 input = Input::GetPadLeftStick(0) * -1;

			if (input.magnitudeSquared() > FLT_EPSILON) {

				Vector3 mov(0, 0, 0);
				if (target)
				{
					if (target->IsDead())
						target = nullptr;
					else
						mov = target->transform->position - player->transform->position;

				}
				else {

					mov -= player->player_camera->transform->AxisX() * input.x * 10.0f;
					mov -= player->player_camera->transform->AxisZ() * input.y * 10.0f;
				}
				mov = ProjectOnPlane(mov, { 0,1,0 });
				rb->SetVelocity(mov);
				player->transform->SetAxisZ(-mov.getNormalized());
			}
		}
	}
	void PlayerCombatComboState::OnExit(IStateMachine* machine)
	{
		if (hit_box)
			hit_box->RemoveThisComponent();
		hit_box.reset();
		Time::SetTimeScale(1.0);
		player->GetScene()->physics_timescale = 1.0f;
		animator->anim_speed = 1.0f;

		target = nullptr;
	}
	void PlayerCombatComboState::Update(IStateMachine* machine, float dt)
	{
		attack_timer += dt;
		if (attack_timer >= ATTACK_TIME) {
			input_limit = true;
			rb->SetVelocity(Vector3(0, rb->velocity.y, 0));
		}
		if (!input_limit && (Input::GetPadButtonDown(0, PadButton::RTrigger) || Input::GetKeyDown(KeyCode::L)))
			next_avalable = true;
		if (hit_stop_timer > 0.0f) {
			hit_stop_timer -= dt;
			if (hit_stop_timer <= 0.0f) {
				animator->anim_speed = 2.0f;
				Time::SetTimeScale(1.0);
				player->GetScene()->physics_timescale = 1.0f;
			}
		}
	}
	void PlayerCombatComboState::OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
	}
	void PlayerCombatComboState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == hit_box.lock()) {
			auto enem = SafeStaticCast<Enemy>(hit_info.hit_collision->owner.lock());

			animator->anim_speed = 0.001f;
			Time::SetTimeScale(0.0);
			player->GetScene()->physics_timescale = 0.0f;
			hit_stop_timer = HIT_STOP_TIME;

			if (knock_back) {
				enem->Down(Vector3(ProjectOnPlane(enem->transform->position - player->transform->position, { 0,1,0 })).getNormalized(), 2);
			}
			else {
				enem->Damage(1);
				if (!target)
					target = enem;

			}
		}
	}
	void PlayerCombatComboState::DebugDraw()
	{
		if (hit_box)
			hit_box->DebugDraw();
		if (target) {
			printfDx("attack! target:%s", target->name.c_str());
		}
	}
}
