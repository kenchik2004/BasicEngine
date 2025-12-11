#include "PlayerSmashChargeState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Managers/LightManager.h"

namespace NeonFade {
	PlayerSmashChargeState::PlayerSmashChargeState(Player* player_)
		:IState(static_cast<GameObject*>(player_))
	{
		owner_player = player_;
		rb = player_->rb.lock().get();
		animator = player_->animator.lock().get();
		std::function<bool()> to_smash = [this]() {
			return charge_timer >= MAX_CHARGE_TIME;
			};
		RegisterChangeRequest("smash_attack", to_smash, 0);
	}
	void PlayerSmashChargeState::OnEnter(IStateMachine* machine)
	{
		charge_timer = 0.0f;
		animator->PlayIfNoSame("smash_charge", true, 0.0f, 1.5f, false);
		rb->use_gravity = false;
		rb->velocity = { 0,0,0 };
		{
			auto eff = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"effect_smash_charge", MAX_CHARGE_TIME);
			auto eff_player = eff->AddComponent<EffectPlayer>(u8"data/FX/Missile.efkefc");
			eff_player->Play();
			charge_effect = eff;
		}
		{
			if (!light_manager)
				light_manager = SceneManager::Object::Get<LightManager>().get();
			auto light = make_safe_shared<PointLight>();
			light->intensity = 0.001f;
			light->range = 50.0f;
			Color light_color = Random::Color({ 0,0,0,1 }, { 1,1,1,1 });
			light_color = light_color * 100.0f;
			light->color = light_color;
			light->position = owner_player->transform->position;
			light_manager->AddLight(light);
			smash_lights.push_back(light);

		}
	}
	void PlayerSmashChargeState::OnExit(IStateMachine* machine)
	{
		rb->use_gravity = true;
		rb->velocity = { 0,0,0 };
		charge_effect.reset();
		for (auto& light : smash_lights) {
			light_manager->RemoveLight(light);
		}
	}
	void PlayerSmashChargeState::Update(IStateMachine* machine, float dt)
	{
		charge_timer += dt;
		Vector3 move_dir = Vector3(0, 0, 0);
		move_dir += owner_player->transform->AxisZ() * -3.0f;
		move_dir += owner_player->transform->AxisY() * 3.0f;
		rb->velocity = move_dir;
		auto pos = MV1GetFramePosition(owner_player->model->GetModelHandle(), 45);
		if (charge_effect)
			charge_effect->transform->position = cast(pos);
		for (auto& light : smash_lights) {
			light->position = cast(pos);

			Color light_color = Random::Color({ 0,0,0,1 }, { 1,1,1,1 });
			light_color = light_color * 100.0f;
			light->color = light_color;
		}
	}
}