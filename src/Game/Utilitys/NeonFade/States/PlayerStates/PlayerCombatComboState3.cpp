#include "PlayerCombatComboState3.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "System/Components/EffectPlayer.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"

namespace NeonFade {
	PlayerCombatComboState3::PlayerCombatComboState3(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		rb = owner_->rb.lock().get();
		animator = owner_->animator.lock().get();
		std::function left_kick215 = [this]() {
			if (hit_box)
				hit_box->RemoveThisComponent();
			knock_back = true;

			auto col = player->AddComponent<SphereCollider>
				(Vector3(0, 0, 4), Quaternion(0, 0, 0, 1), 4.0f,
					true, Collider::Layer::Wepon, Collider::Layer::Enemy);

			hit_box = std::move(col);
			{
				Vector2 input = Input::GetPadLeftStick(0);

				if (input.magnitudeSquared() > FLT_EPSILON) {

					Vector3 mov(0, 0, 0);
					if (target)
					{
						mov = target->transform->position - player->transform->position;
					}
					else {

						mov += player->player_camera->transform->AxisX() * input.x * 10.0f;
						mov += player->player_camera->transform->AxisZ() * input.y * 10.0f;
					}
					mov = ProjectOnPlane(mov, { 0,1,0 });
					rb->SetVelocity(mov.getNormalized());
					player->transform->SetAxisZ(mov.getNormalized());
				}
			}
			};

		animator->SetAnimationCallBack("combat_combo", left_kick215, 215, "last_kick");
		std::function<bool()> to_next = [this]() {
			return input_limit;
			};
		RegisterChangeRequest("idle", to_next, 0);
	}
	void PlayerCombatComboState3::OnEnter(IStateMachine* machine)
	{

		input_limit = false;
		next_avalable = false;
		knock_back = false;
		//rb->velocity = { rb->velocity.x * 0.5f, rb->velocity.y,rb->velocity.z * 0.5f };
		//animator->Play("combat_combo", false, 3.1f, 0.1f);
		animator->anim_speed = 2.0f;
		hit_stop_timer = 0.0f;
		attack_timer = 0.0f;
		{
			Vector2 input = Input::GetPadLeftStick(0);

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

					mov += player->player_camera->transform->AxisX() * input.x * 10.0f;
					mov += player->player_camera->transform->AxisZ() * input.y * 10.0f;
				}
				mov = ProjectOnPlane(mov, { 0,1,0 });
				rb->SetVelocity(mov);
				player->transform->SetAxisZ(mov.getNormalized());
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
		player->GetScene()->physics_timescale = 1.0f;
		animator->anim_speed = 1.0f;

		target = nullptr;
	}
	void PlayerCombatComboState3::Update(IStateMachine* machine, float dt)
	{
		attack_timer += dt;
		if (attack_timer >= ATTACK_TIME) {
			input_limit = true;
			rb->SetVelocity(Vector3(0, rb->velocity.y, 0));
		}
		if (hit_stop_timer > 0.0f) {
			hit_stop_timer -= dt;
			if (hit_stop_timer <= 0.0f) {
				animator->anim_speed = 2.0f;
				Time::SetTimeScale(1.0);
				player->GetScene()->physics_timescale = 1.0f;
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
				enem->Damage(100);
				enem->Down(Vector3(ProjectOnPlane(enem->transform->position - player->transform->position, { 0,1,0 })).getNormalized() * 10);
			}
			else {
				enem->Damage(100);

			}
			if (!target) {
				target = enem;
				{
					auto eff = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"effect", 1.0f);
					eff->transform->position = target->transform->position + Vector3(0, 5, 0);
					eff->transform->rotation = player->player_camera->transform->rotation;
					auto eff_comp = eff->AddComponent<EffectPlayer>("data/FX/Impact.efkefc");
					eff_comp->Play();
				}

			}
		}
	}
}