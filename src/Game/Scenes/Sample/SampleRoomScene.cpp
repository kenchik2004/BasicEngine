#include "precompile.h"
#include "SampleRoomScene.h"
#include "System/Components/ModelRenderer.h"
#include "System/Objects/CameraObject.h"
#include "System/Objects/ShadowMapObject.h"
#include "System/Utils/Render.h"
#include "Game/Objects/Sample/SampleAnimationObject.h"
#include "System/MaterialManager.h"
#include "Game/Managers/LightManager.h"

namespace Sample {

	std::vector<SafeSharedPtr<LightBase>> lights_in_scene;
	void SampleRoomScene::Load()
	{
		loading_status = LOADING_STATUS::LOADING;
		ModelManager::LoadAsModel(u8"data/空色町1.52/sorairo1.52.mv1", "town_model");
		//ModelManager::LoadAsModel(u8"data/BedRoom.mv1", "room_model");
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

		sub_camera_obj = SceneManager::Object::Create<CameraObject>(u8"カメラ1");
		sub_camera_obj->transform->position = { 5.0f, 8.0f, -5.0f };
		sub_camera_obj->transform->rotation = EulerToQuaternion({ 5.0f, -45.0f, 0.0f });
		sub_camera_obj.lock()->status.status_bit.set(ObjStat::STATUS::ACTIVE, false);
		camera_obj = SceneManager::Object::Create<CameraObject>(u8"カメラ");
		camera_obj->GetComponent<Camera>()->render_type = Camera::RenderType::Deferred;
		auto shadow_map = SceneManager::Object::Create<ShadowMapObject>(u8"シャドウマップ");
		shadow_map->SetLightDirection({ 0, -8, 5 });
		shadow_map->SetShadowMapSize(2048);
		auto room = SceneManager::Object::Create<GameObject>("Room");
		SceneManager::Object::Create<SampleAnimationObject>(u8"アニメーションオブジェクト");
		auto room_model = room->AddComponent<ModelRenderer>();
		//room_model->SetModel("room_model");
		room_model->SetModel("town_model");
		//room_model->cast_shadow = false;
		room->transform->scale = { 2,2,2 };
		room_obj = room;
#if 1
		auto light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
		for (int i = 0; i < 1000; i++) {
			auto color = Random::Color(Color::BLACK, Color::WHITE);
			color = color * 5.0f;
			light_manager->AddLight(LightType::Point, Random::Position({ -1000,10.0f,-1000 }, { 1000,20.0f,0 }), color, 70, 0.5f);
		}
		lights_in_scene = light_manager->GetLights();
		light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 10,10,10 }, 0, 0, { 0,-8,5 });

#endif
		return Super::Init();
	}

	void SampleRoomScene::Update()
	{
		if (!CheckForLoading())
			return;
		if (Input::GetKeyDown(KeyCode::P))
		{
			//bool is_active = sub_camera_obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE);
			//sub_camera_obj.lock()->status.status_bit.set(ObjStat::STATUS::ACTIVE, !is_active);
			auto cam_comp = camera_obj->GetComponent<Camera>();
			Camera::RenderType current_type = cam_comp->render_type;
			cam_comp->render_type = current_type == Camera::RenderType::Deferred ? Camera::RenderType::Forward : Camera::RenderType::Deferred;
		}
		//for (auto& light : lights_in_scene)
		//{
		//	if (light->type == LightType::Point) {
		//		auto p_light = static_cast<PointLight*>(light.get());
		//		p_light->range = 5 + sinf(Time::GetTimeFromStart() * 0.2f) * 1.25f;
		//	}
		//}
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

	void SampleRoomScene::OnLateDrawFinish()
	{
		if (!CheckForLoading())
		{
			DrawString(0, 0, u8"Now Loading...", 0xff0000);
		}
		DxLib::DrawFormatString(0, 0, 0xff0000, u8"FPS:%.2f", Time::GetFPS());
	}

	void SampleRoomScene::Exit()
	{
		room_obj.reset();
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