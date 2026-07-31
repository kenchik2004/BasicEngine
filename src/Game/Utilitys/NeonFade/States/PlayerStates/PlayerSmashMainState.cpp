//---------------------------------------------------------------------------
//! @file   PlayerSmashMainState.cpp
//! @brief  PlayerSmashMainStateの実装。プレイヤーのスマッシュメイン状態の処理を行う
//---------------------------------------------------------------------------
#include "PlayerSmashMainState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/GameObjectWithLifeTime.h"
#include "Game/Components/PlayerCameraMachine.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

namespace NeonFade {
	PlayerSmashMainState::PlayerSmashMainState(Player* player_)
		:IState(static_cast<GameObject*>(player_))
	{
		owner_player = player_;
		rb = player_->rb.lock().get();
		animator = player_->animator.lock().get();

		std::function<bool()> to_idle = [this]() {
			return smash_timer >= MAX_SMASH_TIME;
			};
		RegisterChangeRequest("idle", to_idle, 0);
		if (!smash_se)
			smash_se = AudioManager::CloneByName(u8"smash_se");
	}
	void PlayerSmashMainState::OnEnter(IStateMachine* machine)
	{
		smash_timer = 0.0f;
		animator->PlayIfNoSame("smash_attack", true, 0.0f, 0.1f, false);
		anim_after_smash = false;
		smash_se->PlayOneShot(SceneGame::GetSEVolume());
		{

			Vector3 smash_velocity = owner_player->transform->AxisZ() * SMASH_VELOCITY_FORWARD;
			smash_velocity += owner_player->transform->AxisY() * -SMASH_VELOCITY_DOWN;
			rb->velocity = smash_velocity;
			owner_player->player_camera_machine->ShakeCamera(CAMERA_SHAKE_POWER, CAMERA_SHAKE_TIME);
		}
		{
			auto eff = SceneManager::Object::Create<GameObjectWithLifeTime>(u8"effect_smash", 2.0f);
			Vector3 offset = owner_player->transform->AxisZ() * 20.0f;
			eff->move_dir = owner_player->transform->AxisZ() * 70.0f;
			offset += owner_player->transform->AxisY() * 4.0f;
			eff->transform->position = (owner_player->transform->position + offset);
			eff->transform->SetAxisZ(owner_player->transform->AxisZ());
			auto eff_player = eff->AddComponent<EffectPlayer>(u8"data/FX/SonicBoom.efkefc");
			eff_player->Play();
		}
	}
	void PlayerSmashMainState::OnExit(IStateMachine* machine)
	{
		if (hit_box)
		{
			hit_box->RemoveThisComponent();
			hit_box = nullptr;
		}
		owner_player->transform->SetAxisY({ 0,1,0 }, owner_player->transform->AxisX());
		// カメラモードがCINEMATICの場合、MANIPULATEに切り替える
		if (owner_player->player_camera_machine->GetCameraMode() == PlayerCameraMachine::CINEMATIC)
		{
		owner_player->player_camera_machine->camera_distance_max = 30.0f; // カメラ距離を元に戻す
			owner_player->player_camera_machine->SetTransitionTime(0.0f);
			owner_player->player_camera_machine->SetCameraMode(PlayerCameraMachine::CAMERA_MODE::MANIPULATE);
		}
	}
	void PlayerSmashMainState::Update(IStateMachine* machine, float dt)
	{
		smash_timer += dt;

		owner_player->player_camera_machine->SetTransitionTime(0.5f);
		if (smash_timer < 0.5f) {
			owner_player->player_camera_machine->camera_distance_max = std::lerp(50.0f, 17.0f, (0.5f - smash_timer) / 1.0f);
		}
		else if (smash_timer < 0.7f) {

			owner_player->player_camera_machine->camera_distance_max = std::lerp(30.0f, 50.0f, (0.2f - smash_timer + 0.5f) / 1.0f);
			rb->velocity = Vector3(0, 0, 0);
		}

		if (smash_timer >= 0.3f && !anim_after_smash) {

			animator->PlayIfNoSame("smash_finish", false, 0.0f, 0.0f, false);
			anim_after_smash = true;

			if (owner_player->player_camera_machine->GetCameraMode() == PlayerCameraMachine::CINEMATIC)
			{
				owner_player->player_camera_machine->SetTransitionTime(0.2f);
				owner_player->player_camera_machine->SetCameraMode(PlayerCameraMachine::CAMERA_MODE::MANIPULATE);
			}

			{
				auto hit_box_ = owner_player->AddComponent<CapsuleCollider>();
				hit_box_->position = { 20.0f,0,0 };
				hit_box_->rotation = Quaternion(DEG2RAD(-90), { 0,1,0 });
				hit_box_->is_trigger = true;
				hit_box_->radius = 7.5f;
				hit_box_->height = 30.0f;
				hit_box_->SetLayer(Collider::Layer::Wepon);
				hit_box_->SetHitGroup(Collider::Layer::Enemy);
				hit_box = hit_box_;
			}
		}
		if (smash_timer >= 0.7f && hit_box)
		{
			hit_box->RemoveThisComponent();
			hit_box.reset();
		}

	}
	void PlayerSmashMainState::OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)
	{
		if (hit_info.collision == hit_box)
		{
			auto hit_obj = hit_info.hit_collision->owner.lock();
			auto enemy = SafeStaticCast<Enemy>(hit_obj);
			if (enemy) {
				enemy->Damage(SMASH_DAMAGE);
				enemy->Down(owner_player->transform->AxisZ() * 30.0f + Vector3(0, 30, 0));
			}
		}
	}
	void PlayerSmashMainState::DebugDraw()
	{
		Vector3 smash_velocity = owner_player->transform->AxisZ() * SMASH_VELOCITY_FORWARD;
		smash_velocity += owner_player->transform->AxisY() * -SMASH_VELOCITY_DOWN;
		smash_velocity.normalize();
		RayCastInfo ray_info;
		Ray ray;
		ray.direction = smash_velocity;
		ray.position = owner_player->transform->position + Vector3(0, 5.0f, 0);
		ray.length = 100.0f;
		physx::PxQueryFilterData filter_data;
		filter_data.data.word0 = static_cast<u8>(Collider::Layer::Terrain);
		filter_data.flags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::ePREFILTER;
		owner_player->GetScene()->RayCast(ray, ray_info, filter_data);
		if (ray_info.hasBlock) {
			DrawSphere3D(cast(ray_info.block.position), 2.0f, 16, Color::RED, Color::RED, true);

		}

	}
}