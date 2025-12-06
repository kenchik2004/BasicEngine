#include "PlayerAttack1State.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"

namespace NeonFade {
	PlayerAttack1State::PlayerAttack1State(Player* player_)
		:IState(static_cast<GameObject*>(player_))
	{
		owner_player = player_;
		rb = player_->rb.lock().get();
		animator = player_->animator.lock().get();
		std::function<bool()> default_exit =
			[this]() {
			return exit_timer >= EXIT_TIME;
			};
		RegisterChangeRequest("idle", default_exit, 0);

	}
	void PlayerAttack1State::OnEnter(IStateMachine* machine)
	{
		exit_timer = 0.0f;
		animator->Play("spin", true, 0, 0);
		Vector3 input = static_cast<PlayerStateMachine*>(machine)->move_input;
		owner_player->transform->SetAxisZ(input.getNormalized());
		start_rot = owner_player->transform->rotation;
		spin_rot = start_rot * Quaternion(DEG2RAD(45.0f), { 0,0,1 });
		animator->anim_speed = 1.0f;
		rb->velocity = owner_player->transform->AxisZ() * 15 + Vector3(0, 20, 0);
		hit_stop_timer = 0.0f;
		{
			eff = SceneManager::Object::Create<GameObject>(u8"effect_attack1");
			auto eff_player = eff->AddComponent<EffectPlayer>(u8"data/FX/Simple_Trail.efkefc");
			eff_player->Play(true);
			auto pos = MV1GetFramePosition(owner_player->model->GetModelHandle(), 62);
			eff->transform->position = cast(pos);
			eff->transform->scale = Vector3(3, 3, 3);
		}

	}
	void PlayerAttack1State::Update(IStateMachine* machine, float dt)
	{
		exit_timer += dt;


		static constexpr float smoothstep_rottime = 1.0f / SPIN_TIME;
		float cur_animspeed = 0.0f;
		if (exit_timer <= SPIN_TIME) {
			//いきなりスピンし始めたら不自然なので、徐々に加速 1->2
			//ついでにアニメーションが逆回転になっているので、逆転させる
			cur_animspeed = exit_timer * smoothstep_rottime * -2.0f;
			owner_player->transform->rotation = Slerp(start_rot, spin_rot, exit_timer * smoothstep_rottime);
		}
		else {
			animator->PlayIfNoSame("spin_kick", false, 0, 0, true);
			cur_animspeed = 2.5f;
			if (!hit_box) {
				auto col = owner_player->AddComponent<SphereCollider>
					(Vector3(0, 0, 5), Quaternion(0, 0, 0, 1), 5.0f,
						true, Collider::Layer::Wepon, Collider::Layer::Enemy);

				hit_box = std::move(col);
			}
		}
		if (hit_stop_timer > 0.0f) {
			hit_stop_timer -= dt;
			if (hit_stop_timer <= 0.0f)
				animator->anim_speed = cur_animspeed;
		}
		else
			animator->anim_speed = cur_animspeed;
		if (exit_timer >= 0.6f && hit_box) {
			hit_box->RemoveThisComponent();
			hit_box.reset();
		}
		if (eff) {
			auto pos = MV1GetFramePosition(owner_player->model->GetModelHandle(), 62);
			eff->transform->position = cast(pos);
		}
	}
	void PlayerAttack1State::OnExit(IStateMachine* machine)
	{
		owner_player->transform->rotation = start_rot;
		animator->anim_speed = 1.0f;
		if (eff) {
			SceneManager::Object::Destroy(eff);

		}
	}
	void PlayerAttack1State::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == hit_box) {
			auto enem = SafeStaticCast<Enemy>(hit_info.hit_collision->owner.lock());
			if (enem && !enem->IsDead()) {
				Vector3 knockback_dir = enem->transform->position - owner_player->transform->position;
				knockback_dir.y = -3;
				knockback_dir.normalize();
				knockback_dir *= 30;
				enem->Down(knockback_dir, 3);
			}

			if (hit_stop_timer <= 0.0f) {
				hit_stop_timer = HIT_STOP_TIME;
				animator->anim_speed = 0.01f;
			}
		}
	}
}