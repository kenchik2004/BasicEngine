//---------------------------------------------------------------------------
//! @file   PlayerSmashChargeState.cpp
//! @brief  PlayerSmashChargeStateの実装。プレイヤーのスマッシュチャージ状態の処理を行う
//---------------------------------------------------------------------------
#include "PlayerSmashChargeState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Managers/LightManager.h"
#include "Game/Components/PlayerCameraMachine.h"

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
		if (!charge_se)
			charge_se = AudioManager::CloneByName(u8"smash_charge_se");
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

		charge_se->PlayOneShot();

		//ビリビリしてそうな感じの動画テクスチャをマテリアルにセット
		{
			owner_player->SetElectroEffectTextureToMaterials();
		}
		{
			if (!light_manager)
				light_manager = SceneManager::Object::Get<LightManager>().get();
			Color light_color = Random::Color({ 0,0,0,1 }, { 1,1,1,1 });
			light_color = light_color * 10000.0f;
			static const float intensity = 0.001f;
			static const float range = 50.0f;
			auto light = make_safe_shared<PointLight>(owner_player->transform->position, light_color, intensity, range);
			light_manager->AddLight(light);
			smash_lights.push_back(light);

		}

		auto& camera_machine = owner_player->player_camera_machine;
		if (Random::Int(0, 5) == 0) {
			camera_machine->SetTransitionTime(1.0f);
			camera_machine->SetCameraMode(PlayerCameraMachine::CAMERA_MODE::CINEMATIC);
			Vector3 cinematic_offset = -owner_player->transform->AxisX();
			cinematic_offset += owner_player->transform->AxisY() * 0.5f;
			cinematic_offset += owner_player->transform->AxisZ() * -1.7f;
			camera_machine->SetCinematicOffset(cinematic_offset.getNormalized());
			camera_machine->camera_distance_max = 17.0f;
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
		auto mat = owner_player->model->GetFrameWorldMat(81);
		if (charge_effect)
			charge_effect->transform->position = cast(mat.getPosition());
		for (auto& light : smash_lights) {
			light->position = cast(mat.getPosition());

			Color light_color = Random::Color({ 0,0,0,1 }, { 1,1,1,1 });
			light_color = light_color * 10000.0f;
			light->color = light_color;
		}
	}
}