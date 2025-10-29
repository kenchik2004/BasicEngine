#include "precompile.h"
#include "SampleMaterialScene.h"
#include "System/Utils/Render.h"
#include "System/Components/Camera.h"
#include "System/Objects/CameraObject.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/Animator.h"
#include "System/MaterialManager.h"
#include "System/Objects/ShadowMapObject.h"
namespace Sample
{
	void SampleMaterialScene::Load()
	{
		ModelManager::LoadAsModel(u8"data/TV/TV.mv1", "tv_model");
		ModelManager::LoadAsModel(u8"data/Stage/SwordBout/Stage00.mv1", "ground");
		ModelManager::LoadAsModel(u8"data/player/model.mv1", "anim_model");
		ModelManager::LoadAsAnimation(u8"data/player/anim_dance.mv1", "dance");
		// テレビの画面用テクスチャを作成
		tv_screen_texture = make_safe_shared<Texture>(SCREEN_W, SCREEN_H, DXGI_FORMAT_R8G8B8A8_UNORM);
		player_override_texture = make_safe_shared<Texture>(SCREEN_W, SCREEN_H, DXGI_FORMAT_R8G8B8A8_UNORM);
		CheckForLoading();
	}

	int SampleMaterialScene::Init()
	{
		if (!CheckForLoading())
			return Super::Init();
		SceneManager::Object::Create<ShadowMapObject>("shadow_map_object");
		auto ground_obj = SceneManager::Object::Create<GameObject>("ground_object");
		ground_obj->transform->scale = { 0.05f,0.05f,0.05f };

		auto ground_model_cmp = ground_obj->AddComponent<ModelRenderer>();
		ground_model_cmp->SetModel("ground");
		auto tv_obj = SceneManager::Object::Create<GameObject>("tv_object");
		auto tv_model_cmp = tv_obj->AddComponent<ModelRenderer>();
		tv_screen_mat = MaterialManager::CreateMaterial("tv_screen_mat");
		tv_screen_mat->SetTexture(tv_screen_texture, Material::TextureType::Diffuse);
		tv_model_cmp->SetModel("tv_model");
		tv_model_cmp->SetMaterial(tv_screen_mat, 1);
		tv_obj->transform->SetPosition({ 7,5,20 });
		tv_obj->transform->scale = { 0.05f,0.05f,0.05f };
		tv_obj->transform->rotation = Quaternion(DEG2RAD(45), { 0,1,0 });
		SceneManager::Object::Create<CameraObject>("camera_object")->transform->SetPosition({ 0,10,-10 });

		auto anim_obj = SceneManager::Object::Create<GameObject>("anim_object");
		auto anim_model_cmp = anim_obj->AddComponent<ModelRenderer>();
		anim_model_cmp->SetModel("anim_model");
		auto anim_cmp = anim_obj->AddComponent<Animator>();
		anim_cmp->SetAnimation("dance", 0);
		anim_obj->transform->SetPosition({ -7,5,20 });
		anim_obj->transform->scale = { 0.05f,0.05f,0.05f };
		anim_cmp->Play("dance", true);
		player_model = anim_model_cmp;

		auto rt = GetRenderTarget();
		SetRenderTarget(player_override_texture.get(), nullptr);
		ClearColor({ 0,0,0,1 });
		for (int y = 0; y < SCREEN_H; y += 20)
			for (int x = 0; x < SCREEN_W; x += 100)
				DrawString(x, y, u8"Overrided ", 0xffffff);
		SetRenderTarget(rt);
		player_override_mat = MaterialManager::CreateMaterial("player_override_mat");
		player_override_mat->SetTexture(player_override_texture, Material::TextureType::Diffuse);

		player_default_mat = anim_model_cmp->GetMaterial(0);




		return 0;
	}

	void SampleMaterialScene::Update()
	{
		if (!CheckForLoading())
			return;
		auto cam = GetCurrentCamera()->owner;
		if (Input::GetKey(KeyCode::W))
			cam->transform->MovePosition({ 0,0,10 });
		if (Input::GetKey(KeyCode::S))
			cam->transform->MovePosition({ 0,0,-10 });
		if (Input::GetKey(KeyCode::A))
			cam->transform->MovePosition({ -10,0,0 });
		if (Input::GetKey(KeyCode::D))
			cam->transform->MovePosition({ 10,0,0 });
		if (Input::GetKey(KeyCode::Q))
			cam->transform->MovePosition({ 0,10,0 });
		if (Input::GetKey(KeyCode::E))
			cam->transform->MovePosition({ 0,-10,0 });

		if (Input::GetKeyDown(KeyCode::Space))
		{
			default_texture_enabled = !default_texture_enabled;
			if (auto player_model_lock = player_model.lock())
			{
				if (default_texture_enabled)
				{
					player_model_lock->SetMaterial(player_default_mat, 0);

				}
				else
				{
					player_model_lock->SetMaterial(player_override_mat, 0);
				}
			}
		}

	}
	bool SampleMaterialScene::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADING) {
			loading_status = (ModelManager::GetLoadingCount() == 0 && AudioManager::GetLoadingCount() == 0) ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
			if (loading_status == LOADING_STATUS::LOADED) {
				Init();
				return true;
			}
			return false;
		}
		return true;
	}

	void SampleMaterialScene::LateDraw()
	{
		if (!CheckForLoading()) {
			DrawFormatString(100, 100, 0xff0000, "Now Loading...");
			return;
		}
		DrawString(0, 0, u8"スペースキーを押してプレイヤーのテクスチャ(マテリアル)を切り替えます", 0xffff00);
		DrawString(0, 20, u8"WASDQEキーでカメラ移動", 0xffff00);
	}

	void SampleMaterialScene::OnDrawFinish()
	{
		if (!CheckForLoading())
			return;
		// カメラの描画結果をテレビの画面テクスチャにコピーする
		auto rt = GetRenderTarget();
		SetRenderTarget(tv_screen_texture.get(), nullptr);
		CopyToRenderTarget(tv_screen_texture.get(), GetCurrentCamera()->hdr.get());
		SetRenderTarget(rt);
	}


	void SampleMaterialScene::Exit()
	{
		tv_screen_texture.reset();
		player_override_texture.reset();
	}
}