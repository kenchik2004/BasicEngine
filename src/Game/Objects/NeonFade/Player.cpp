//---------------------------------------------------------------------------
//! @file   Player.cpp
//! @brief  Playerオブジェクトの実装。プレイヤーキャラクターのゲームロジックを管理する
//---------------------------------------------------------------------------
#include "Player.h"
#include "System/Components/RigidBody.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/Animator.h"
#include "Game/Components/PlayerController.h"
#include "System/Objects/CameraObject.h"


namespace NeonFade
{
	int Player::Init()
	{
		rb = AddComponent<RigidBody>();
		rb->GetBody()->is<physx::PxRigidDynamic>()->setSleepThreshold(0.0f);
		auto model_obj = SceneManager::Object::Create<GameObject>("pl_model");
		model_obj->transform->SetParent(transform);
		model_obj->transform->scale = { 0.05f,0.05f,0.05f };
		model_obj->transform->local_position = { 0,-4.5f,0 };
		model_obj->transform->local_rotation = Quaternion(DEG2RAD(180), { 0,1,0 });
		model = model_obj->AddComponent<ModelRenderer>();
		animator = model_obj->AddComponent<Animator>();

		model->SetModel("player_model");
		animator->SetAnimation("idle", 1);
		animator->SetAnimation("walk", 1);
		animator->SetAnimation("run", 1);
		animator->SetAnimation("sprint", 0);
		animator->SetAnimation("jump", 0);
		animator->SetAnimation("fall_loop", 0);
		animator->SetAnimation("combat_combo", 0);
		animator->SetAnimation("climb", 0);
		animator->SetAnimation("jump_attack", 0);
		animator->SetAnimation("back_flip", 0);
		animator->SetAnimation("player_damage", 0);
		animator->SetAnimation("smash_charge", 0);
		animator->SetAnimation("smash_attack", 0);
		animator->SetAnimation("smash_finish", 0);
		animator->SetAnimation("spin", 0);
		animator->SetAnimation("spin_kick", 0);
		animator->SetAnimation("clouch_inv", 0);
		animator->SetAnimation("leg_sweep", 0);
		animator->SetAnimation("player_dive", 0);
#if 1
		mov_tex = TextureManager::Get("electro_movie");
		//PlayMovieToGraph(*mov_tex, DX_PLAYTYPE_LOOP);
#endif


		rb->freeze_rotation = { 1,1,1 };
		auto col = AddComponent<CapsuleCollider>();
		col->height = 5.7f;
		col->radius = 1.5f;
		//col->position = { 4.5f,0,0 };
		col->rotation = Quaternion(DEG2RAD(90), { 0,0,-1 });
		col->SetLayer(Collider::Layer::Player);
		pl_controller = AddComponent<PlayerController>();

		default_mat = model->GetMaterial(0);
		electro_mat = MaterialManager::CreateMaterial("electro_effect_mat", default_mat);
		electro_mat->SetTexture(mov_tex, Material::TextureType::Emission);


		return 0;
	}
	void Player::Update()
	{
		if (Input::GetKeyDown(KeyCode::Period))
			SetGlobalAmbientLight({ 0,0,0,0 });
		if (transform->position.y < -50)
			transform->position.y = 5.0f;
	}
	void Player::PreDraw()
	{
		UpdateMovieToGraph(*mov_tex);

	}
	void Player::LateDebugDraw()
	{

	}
	void Player::Exit()
	{
		mov_tex.reset();
	}
	void Player::OnCollisionEnter(const HitInfo& hit_info)
	{
		pl_controller->OnCollisionEnter(hit_info);
	}
	void Player::OnCollisionStay(const HitInfo& hit_info)
	{
		pl_controller->OnCollisionStay(hit_info);
	}
	void Player::OnCollisionExit(const HitInfo& hit_info)
	{
		pl_controller->OnCollisionExit(hit_info);
	}
	void Player::OnTriggerEnter(const HitInfo& hit_info)
	{
		pl_controller->OnTriggerEnter(hit_info);
	}
	void Player::OnTriggerStay(const HitInfo& hit_info)
	{
		pl_controller->OnTriggerStay(hit_info);
	}
	void Player::OnTriggerExit(const HitInfo& hit_info)
	{
		pl_controller->OnTriggerExit(hit_info);
	}

	void Player::Damage(int damage)
	{
		pl_controller->Damage(1);
	}
	void Player::SetElectroEffectTextureToMaterials()
	{

		model->SetMaterial(electro_mat, 0);
	}
	void Player::ResetMaterialsToDefault()
	{
		model->SetMaterial(default_mat, 0);
	}
} // namespace NeonFade