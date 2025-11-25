#include "PlayerCombatComboState3.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
namespace NeonFade {
	PlayerCombatComboState3::PlayerCombatComboState3(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		rb = owner_->rb.lock().get();
		animator = owner_->animator.lock().get();
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
			animator->anim_speed = 2.0f;
			Time::SetTimeScale(1.0);
			};
	}
	void PlayerCombatComboState3::OnEnter(IStateMachine* machine)
	{

		input_limit = false;
		next_avalable = false;
		knock_back = false;
		//rb->velocity = { rb->velocity.x * 0.5f, rb->velocity.y,rb->velocity.z * 0.5f };
		animator->Play("combat_combo", false, 3.1f, 0.1f);
		animator->anim_speed = 2.0f;
		hit_stop_timer = 0.0f;
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
	void PlayerCombatComboState3::OnExit(IStateMachine* machine)
	{
		animator->Stop();
		if (hit_box)
			hit_box->RemoveThisComponent();
		hit_box.reset();
		Time::SetTimeScale(1.0);
		player->GetScene()->physics_timescale = 2.0f;
		animator->anim_speed = 1.0f;

		target = nullptr;
	}
	void PlayerCombatComboState3::Update(IStateMachine* machine, float dt)
	{
		if (!input_limit && (Input::GetPadButtonDown(0, PadButton::Fuga) || Input::GetKeyDown(KeyCode::L)))
			next_avalable = true;
		if (hit_stop_timer > 0.0f) {
			hit_stop_timer -= dt;
			if (hit_stop_timer <= 0.0f) {
				animator->anim_speed = 2.0f;
				Time::SetTimeScale(1.0);
				player->GetScene()->physics_timescale = 2.0f;
			}
		}
	}
	void PlayerCombatComboState3::OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
	}
	void PlayerCombatComboState3::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
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
}