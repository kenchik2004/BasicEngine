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
#if 1
		auto mov_tex = TextureManager::Get("electro_movie");
#endif
		int mv1_handle = model->GetModelHandle();
		auto mat = model->GetMaterial(0);
		auto mat2 = model->GetMaterial(1);
		//MV1SetMaterialEmiColor(mv1_handle, 0, { 10.0f,10.0f,10.0f,10.0f });
		//mat->SetTexture(TextureManager::Get("electro_texture"), Material::TextureType::Emission);
		//mat2->SetTexture(TextureManager::Get("electro_texture"), Material::TextureType::Emission);
		//mat->SetTexture(TextureManager::Create("electro_texture", 1000, 1000, DXGI_FORMAT_B8G8R8A8_UNORM), Material::TextureType::Emission);

		PlayMovieToGraph(*mov_tex, DX_PLAYTYPE_LOOP);


		mat->SetTexture(mov_tex, Material::TextureType::Emission);
		mat2->SetTexture(mov_tex, Material::TextureType::Emission);
		rb->freeze_rotation = { 1,1,1 };
		auto col = AddComponent<CapsuleCollider>();
		col->height = 5.7f;
		col->radius = 1.5f;
		//col->position = { 4.5f,0,0 };
		col->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
		col->SetLayer(Collider::Layer::Player);
		pl_controller = AddComponent<PlayerController>();
		//auto foot_col = AddComponent<BoxCollider>();
		//foot_col->extension = { 1.4f,0.5f,1.4f };
		//foot_col->position = { 0,0.0f,0 };
		//foot_col->SetLayer(Collider::Layer::Player);
		//foot_col->SetHitGroup(Collider::Layer::Terrain);
		//foot = foot_col;
		//AddComponent<AudioListener>();

		return 0;
	}
	void Player::Update()
	{
		if (Input::GetKeyDown(KeyCode::Period))
			SetGlobalAmbientLight({ 0,0,0,0 });
	}
	void Player::PreDraw()
	{
		auto mat = model->GetMaterial(0);
		auto mov_tex = mat->GetTexture(Material::TextureType::Emission);
		UpdateMovieToGraph(*mov_tex);

#if 0
		auto cur_rt = GetRenderTarget();
		auto mat = model->GetMaterial(0);
		auto tex = mat->GetTexture(Material::TextureType::Emission);
		SetRenderTarget(tex.get());
		SetDrawScreen(*tex);
		ClearColor({ 0,0,0,0 });
		PlayMovieToGraph(movie, DX_PLAYTYPE_LOOP);
		DrawExtendGraph(0, 0, tex->Width(), tex->Height(), movie, TRUE);
		SetDrawScreen(DX_SCREEN_BACK);
		SetRenderTarget(cur_rt);
#endif
	}
	void Player::LateDebugDraw()
	{

	}
	void Player::Exit()
	{
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
} // namespace NeonFade