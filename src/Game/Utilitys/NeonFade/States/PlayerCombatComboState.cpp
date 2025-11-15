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
		std::function slip_stop2 = [this]() {
			rb->velocity = { 0,rb->velocity.y,0 };
			input_limit = true;
			};
		std::function slip_stop3 = [this]() {
			rb->velocity = { 0,rb->velocity.y,0 };
			input_limit = true;
			};
		std::function slip_stop4 = [this]() {
			rb->velocity = { 0,rb->velocity.y,0 };
			};

		std::function left_punch85 = [this]() {
			auto col = player->AddComponent<SphereCollider>();
			col->SetHitGroup(Collider::Layer::Enemy);
			col->SetLayer(Collider::Layer::Wepon);
			col->is_trigger = true;
			col->radius = 3;
			col->position = { 0,5,-5 };
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
		std::function left_punch85_fin = [this]() {
			animator->anim_speed = 3.0f;
			Time::SetTimeScale(1.0);
			player->GetScene()->physics_timescale = 2.0f;

			};
		std::function left_punch160 = [this]() {
			if (hit_box)
				hit_box->RemoveThisComponent();

			auto col = player->AddComponent<SphereCollider>();
			col->SetHitGroup(Collider::Layer::Enemy);
			col->SetLayer(Collider::Layer::Wepon);
			col->is_trigger = true;
			col->radius = 5;
			col->position = { 0,5,-3 };
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
			animator->anim_speed = 3.0f;
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
			col->radius = 5;
			col->is_trigger = true;
			col->position = { 0,5,-3 };
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
			animator->anim_speed = 3.0f;
			Time::SetTimeScale(1.0);
			};

		animator->SetAnimationCallBack("combat_combo", slip_stop, 40, "slip_stop");
		animator->SetAnimationCallBack("combat_combo", left_punch85, 88, "first_punch");
		animator->SetAnimationCallBack("combat_combo", left_punch85_fin, 90, "first_punch_fin");
		animator->SetAnimationCallBack("combat_combo", slip_stop2, 92, "slip_stop2");
		animator->SetAnimationCallBack("combat_combo", left_punch160, 160, "second_punch");
		animator->SetAnimationCallBack("combat_combo", left_punch160_fin, 164, "second_punch_fin");
		animator->SetAnimationCallBack("combat_combo", slip_stop3, 166, "slip_stop3");
		animator->SetAnimationCallBack("combat_combo", left_kick210, 210, "last_kick");
		animator->SetAnimationCallBack("combat_combo", left_kick210_fin, 213, "last_kick_fin");
		animator->SetAnimationCallBack("combat_combo", slip_stop4, 215, "slip_stop4");
	}
	void PlayerCombatComboState::OnEnter(IStateMachine* machine)
	{
		knock_back = false;
		//rb->velocity = { rb->velocity.x * 0.5f, rb->velocity.y,rb->velocity.z * 0.5f };
		animator->Play("combat_combo", false, 0.2f, 0.1f);
		animator->anim_speed = 3;
		{
			Vector2 input = Input::GetPadLeftStick(0) * -1;

			if (input.magnitudeSquared() > FLT_EPSILON) {

				Vector3 mov(0, 0, 0);
				mov += player->player_camera->transform->AxisX() * input.x;
				mov += player->player_camera->transform->AxisZ() * input.y;
				mov = ProjectOnPlane(mov, { 0,1,0 });
				rb->AddForce(mov.getNormalized() * -5);
				player->transform->SetAxisZ(mov);
			}
		}
	}
	void PlayerCombatComboState::OnExit(IStateMachine* machine)
	{
		animator->Stop();
		if (hit_box)
			hit_box->RemoveThisComponent();
		hit_box.reset();
		Time::SetTimeScale(1.0);
		player->GetScene()->physics_timescale = 2.0f;
		animator->anim_speed = 1.0f;

		input_limit = false;
		next_avalable = false;
		knock_back = false;
		target = nullptr;
	}
	void PlayerCombatComboState::Update(IStateMachine* machine, float dt)
	{
		if (!animator->IsPlaying())
			machine->ChangeState("idle");
		if (Input::GetPadButtonDown(0, PadButton::Fuga) || Input::GetKeyDown(KeyCode::L))
			next_avalable = true;
		if (!next_avalable && input_limit) {
			machine->ChangeState("idle");
		}
		else if (input_limit) {
			input_limit = false;
			next_avalable = false;
		}
	}
	void PlayerCombatComboState::OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
	}
	void PlayerCombatComboState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == hit_box.lock()) {
			auto enem = SafeStaticCast<Enemy>(hit_info.hit_collision->owner.lock());
			if (knock_back)
				enem->Down(Vector3(ProjectOnPlane(enem->transform->position - player->transform->position, { 0,1,0 })).getNormalized());
			else {
				enem->Damage(1);
				if (!target)
					target = enem.get();

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
