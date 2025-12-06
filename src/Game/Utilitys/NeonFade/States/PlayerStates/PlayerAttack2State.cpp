#include "PlayerAttack2State.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"

namespace NeonFade {
	PlayerAttack2State::PlayerAttack2State(Player* player_)
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
	void PlayerAttack2State::OnEnter(IStateMachine* machine)
	{
		exit_timer = 0.0f;
		animator->Play("clouch_inv", false, CUR_ANIMTIME_MAX, 0.1f);
		//mixamoのアニメーションが酷いので、手動で逆再生
		animator->anim_speed = -3.0f;
		effect_played = false;
	}
	void PlayerAttack2State::Update(IStateMachine* machine, float dt)
	{
		exit_timer += dt;
		if (exit_timer >= 0.3f && !hit_box) {
			auto col = owner_player->AddComponent<BoxCollider>
				(Vector3(0, -3, 3), Quaternion(0, 0, 0, 1), Vector3(8, 2, 3),
					true, Collider::Layer::Wepon, Collider::Layer::Enemy);
			hit_box = std::move(col);
			if (!effect_played) {
				auto eff = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"effect_attack2", 3.5f);
				auto eff_player = eff->AddComponent<EffectPlayer>(u8"data/FX/zubaba.efk");

				eff_player->Play();
				Vector3 offset = owner_player->transform->AxisZ() * 2.0f;
				offset += owner_player->transform->AxisY() * -2.0f;
				eff->transform->position = (owner_player->transform->position + offset);
				eff->transform->SetAxisZ(-owner_player->transform->AxisZ());
				effect_played = true;
			}

		}
		if (exit_timer < 0.6f && hit_box) {
			static constexpr float smoothstep = 1.0f / 0.6f;
			hit_box->extension = Vector3(8, 2, 3 + 200.0f * std::lerp<float>(0, 1, smoothstep * (exit_timer - 0.3f)));
			hit_box->position = Vector3(0, -3, 3 + 100.0f * std::lerp<float>(0, 1, smoothstep * (exit_timer - 0.3f)));
		}
		if (exit_timer >= 0.9f && hit_box) {
			hit_box->RemoveThisComponent();
			hit_box.reset();
		}
	}
	void PlayerAttack2State::OnExit(IStateMachine* machine)
	{
		animator->anim_speed = 1.0f;
		if (hit_box) {
			hit_box->RemoveThisComponent();
			hit_box.reset();
		}

	}
	void PlayerAttack2State::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == hit_box) {
			if (auto enemy = SafeDynamicCast<Enemy>(hit_info.hit_collision->owner.lock())) {
				enemy->Damage(2);
			}
		}
	}
}
