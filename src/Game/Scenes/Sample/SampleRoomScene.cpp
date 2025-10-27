#include "precompile.h"
#include "SampleRoomScene.h"
#include "System/Components/ModelRenderer.h"
#include "System/Objects/CameraObject.h"
#include "System/Objects/ShadowMapObject.h"
#include "System/Utils/Render.h"
#include "Game/Objects/Sample/SampleAnimationObject.h"
#include "System/MaterialManager.h"

namespace Sample {
	SafeSharedPtr<Texture> cam_cache_texture = nullptr;
	int draw_count = 0;

	void SampleRoomScene::Load()
	{
		loading_status = LOADING_STATUS::LOADING;
		ModelManager::LoadAsModel(u8"data/BedRoom.mv1", "room_model");
		ModelManager::LoadAsModel("data/player/model.mv1", "player_model");
		ModelManager::LoadAsAnimation("data/player/anim_stand.mv1", "idle");
		ModelManager::LoadAsAnimation("data/player/anim_walk.mv1", "walk");
		ModelManager::LoadAsAnimation("data/player/anim_run.mv1", "run");
		ModelManager::LoadAsAnimation("data/player/anim_jump.mv1", "jump");
		ModelManager::LoadAsAnimation("data/player/anim_salute.mv1", "salute");
		ModelManager::LoadAsAnimation("data/player/anim_aim.mv1", "aim");
		ModelManager::LoadAsAnimation("data/player/anim_reload.mv1", "reload");
		AudioManager::Load("data/Sound/reload.mp3", "reload_se", false);
		CheckForLoading();
	}
	int SampleRoomScene::Init()
	{
		if (!CheckForLoading())
			return Super::Init();
		cam_cache_texture = make_safe_shared<Texture>(800, 90, DXGI_FORMAT_R8G8B8A8_UNORM);
		sub_camera_obj = SceneManager::Object::Create<CameraObject>(u8"カメラ1");
		sub_camera_obj->transform->position = { 5.0f, 8.0f, -5.0f };
		sub_camera_obj->transform->rotation = EulerToQuaternion({ 5.0f, -45.0f, 0.0f });
		camera_obj = SceneManager::Object::Create<CameraObject>(u8"カメラ");
		SceneManager::Object::Create<ShadowMapObject>(u8"シャドウマップ");
		auto room = SceneManager::Object::Create<GameObject>("Room");
		SceneManager::Object::Create<SampleAnimationObject>(u8"アニメーションオブジェクト");
		room->AddComponent<ModelRenderer>()->SetModel("room_model");
		room->transform->scale = { 0.05f,0.05f,0.05f };
		room->transform->position = { 0.0f,-5.0f,0.0f };
		room_obj = room;
		return Super::Init();
	}

	void SampleRoomScene::Update()
	{
		draw_count = 0;
		if (!CheckForLoading())
			return;
		if (Input::GetKeyDown(KeyCode::P))
		{
			bool is_active = sub_camera_obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE);
			sub_camera_obj.lock()->status.status_bit.set(ObjStat::STATUS::ACTIVE, !is_active);
		}

		if (Input::GetMouseButtonRepeat(MouseButton::ButtonRight)) {
			auto cam_lock = camera_obj.lock();
			auto cam_trns = cam_lock->transform;
			if (Input::GetKey(KeyCode::W))
				cam_trns->position += (cam_trns->AxisZ() * 10 * Time::UnscaledDeltaTime());
			if (Input::GetKey(KeyCode::S))
				cam_trns->position += (-cam_trns->AxisZ() * 10 * Time::UnscaledDeltaTime());
			if (Input::GetKey(KeyCode::A))
				cam_trns->position += (-cam_trns->AxisX() * 10 * Time::UnscaledDeltaTime());
			if (Input::GetKey(KeyCode::D))
				cam_trns->position += (cam_trns->AxisX() * 10 * Time::UnscaledDeltaTime());
			if (Input::GetKey(KeyCode::Q))
				cam_trns->position += (-cam_trns->AxisY() * 10 * Time::UnscaledDeltaTime());
			if (Input::GetKey(KeyCode::E))
				cam_trns->position += (cam_trns->AxisY() * 10 * Time::UnscaledDeltaTime());
			Vector2 mouse_move = Input::GetMouseDelta();
			Quaternion qx = Quaternion(mouse_move.x * Time::UnscaledDeltaTime() * 0.2f, { 0,1,0 });
			Quaternion qy = Quaternion(mouse_move.y * Time::UnscaledDeltaTime() * 0.2f, { 1,0,0 });
			cam_trns->AddRotation(qx * qy);

			//X軸を水平に保つ(ヨー方向の回転を打ち消す)
			//		
			//	  			   Y	  
			//				   |	 
			//		___________|__________	
			//	  ／			   |		  \
			//	/			 ┌─-──┐         \
			// /			 |cam ＜---------- Z 
			// |			 └──-─┘			/
			//	\			 /			   /
			//	 \		  ← / →          ／
			//    \_______ /___________／		
			//			  X
			//X軸はグローバルY軸に対して垂直になるように補正
			//->XZ平面上に投影すれば、水平になる

			Vector3 axis_x_proj = cam_trns->AxisX();

			//Y軸に対して垂直な関係にある面(=XZ平面)に投影
			//処理としては、Y軸に投影したときのベクトルを元のベクトルから引くだけ


			axis_x_proj = ProjectOnPlane(axis_x_proj, { 0,1,0 });

			cam_trns->SetAxisX(axis_x_proj.getNormalized(), cam_trns->AxisY());
		}

	}

	void SampleRoomScene::Draw()
	{

	}

	void SampleRoomScene::LateDraw()
	{
		if (!CheckForLoading())
		{
			DrawString(0, 0, u8"Now Loading...", 0xff0000);
		}
		{
			//カメラ映像をキャッシュ
			auto cur_rt = GetRenderTarget();
			SetRenderTarget(cam_cache_texture.get(), nullptr);
			ClearColor(Color::BLACK);
			auto cam_comp = camera_obj.lock()->GetComponent<Camera>();
			std::array<Texture*, 5> textures = { cam_comp->my_screen.get(), cam_comp->gbuffer0.get(),cam_comp->gbuffer1.get(), cam_comp->gbuffer2.get(),cam_comp->my_screen_depth.get() };
			for (int i = 0; i < 5; i++)
			{
				Vector3 draw_pos = { i * SCREEN_W * 0.2f,0,0 };
				Vector3 scale = { SCREEN_W * 0.2f,(float)SCREEN_H,0 };
				VERTEX2DSHADER vert[4];
				u8 alpha_u8 = 255;
				vert[0].pos.x = draw_pos.x;
				vert[0].pos.y = draw_pos.y;
				vert[0].pos.z = 0.0f;
				vert[0].rhw = 1.0f;
				vert[0].dif = { 255,255,255,alpha_u8 };
				vert[0].u = 0.0f;
				vert[0].v = 0.0f;
				vert[1].pos.x = draw_pos.x + scale.x;
				vert[1].pos.y = draw_pos.y;
				vert[1].pos.z = 0.0f;
				vert[1].rhw = 1.0f;
				vert[1].dif = { 255,255,255,alpha_u8 };
				vert[1].u = 1.0f;
				vert[1].v = 0.0f;
				vert[2].pos.x = draw_pos.x;
				vert[2].pos.y = draw_pos.y + scale.y;
				vert[2].pos.z = 0.0f;
				vert[2].rhw = 1.0f;
				vert[2].dif = { 255,255,255,alpha_u8 };
				vert[2].u = 0.0f;
				vert[2].v = 1.0f;
				vert[3].pos.x = draw_pos.x + scale.x;
				vert[3].pos.y = draw_pos.y + scale.y;
				vert[3].pos.z = 0.0f;
				vert[3].rhw = 1.0f;
				vert[3].dif = { 255,255,255,alpha_u8 };
				vert[3].u = 1.0f;
				vert[3].v = 1.0f;
				for (u32 i = 0; i < 16; i++)
				{
					SetUseTextureToShader(i, -1);
				}

				SetTexture(0, textures[i]);

				SetUsePixelShader(*MaterialManager::GetDefaultMat2D()->GetPixelShader());
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				DrawPrimitive2DToShader(vert, 4, DX_PRIMTYPE_TRIANGLESTRIP);
				SetUsePixelShader(-1);
				for (u32 i = 0; i < static_cast<u32>(Material::TextureType::Max); i++) {
					SetUseTextureToShader(i, -1);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				SetDrawMode(DX_DRAWMODE_NEAREST);
			}


			SetRenderTarget(cur_rt);
			DrawGraph(0, 0, *cam_cache_texture, FALSE);

		}
		DrawFormatString(0, 0, 0xff0000, u8"FPS:%.2f", Time::GetFPS());
	}

	void SampleRoomScene::Exit()
	{
		room_obj.reset();
		cam_cache_texture.reset();
		Super::Exit();
	}

	bool SampleRoomScene::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADED)
			return true;
		loading_status = ModelManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADED)
			Init();
		return loading_status == LOADING_STATUS::LOADED;
	}


}