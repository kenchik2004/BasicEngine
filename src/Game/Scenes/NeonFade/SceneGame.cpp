#include "SceneGame.h"

#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/PoliceCar.h"

#include "Game/Managers/LightManager.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"


namespace NeonFade {
	//1ビルの縦横...100m×100m
	std::array<Vector3, 4> buildings = {
		Vector3(-90.0f,0.0f,280.0f),
		Vector3(-90.0f,0.0f,120.0f),
		Vector3(90.0f,0.0f,120.0f),
		Vector3(90.0f,0.0f,280.0f),
	};

	void SceneGame::Load()
	{

		loading_status = LOADING_STATUS::LOADING;
		ModelManager::LoadAsModel(u8"data/player/model.mv1", "player_model");
		ModelManager::LoadAsAnimation(u8"data/player/anim_stand.mv1", "idle");
		ModelManager::LoadAsAnimation(u8"data/player/anim_walk.mv1", "walk");
		ModelManager::LoadAsAnimation(u8"data/player/anim_run.mv1", "run");
		ModelManager::LoadAsAnimation(u8"data/player/anim_sprint.mv1", "sprint");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump.mv1", "jump");
		ModelManager::LoadAsAnimation(u8"data/player/anim_fall_loop.mv1", "fall_loop");
		ModelManager::LoadAsAnimation(u8"data/player/anim_combo.mv1", "combat_combo");
		ModelManager::LoadAsAnimation(u8"data/player/anim_climb.mv1", "climb");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump_attack.mv1", "jump_attack");
		ModelManager::LoadAsAnimation(u8"data/player/anim_back_flip.mv1", "back_flip");
		ModelManager::LoadAsAnimation(u8"data/player/anim_damage.mv1", "player_damage");
		ModelManager::LoadAsAnimation(u8"data/player/anim_punch_charge.mv1", "smash_charge");
		ModelManager::LoadAsAnimation(u8"data/player/anim_punch_strike.mv1", "smash_attack");
		ModelManager::LoadAsAnimation(u8"data/player/anim_punch_finish.mv1", "smash_finish");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/Ground.mv1", "stage");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/building-01_UV.mv1", "building");
		ModelManager::LoadAsModel(u8"data/Stage/megapolis/road.mv1", "high-way");
		ModelManager::LoadAsModel(u8"data/car/police.mv1", "police");


		ModelManager::LoadAsModel(u8"data/enemy/model.mv1", "enemy_model");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_fighting_idle.mv1", "enemy_idle");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_damage.mv1", "enemy_damage");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_down.mv1", "enemy_down");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_t_pose.mv1", "enemy_die");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_attack_charge.mv1", "enemy_attack_charge");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_attack_main.mv1", "enemy_attack_main");
		AudioManager::Load(u8"data/Sound/siren.mp3", "siren");
		TextureManager::Load(u8"data/FX.png", "fx_texture");

		CheckForLoading();
	}

	int SceneGame::Init()
	{
		if (!CheckForLoading())
			return 0;
		auto shadowmap = SceneManager::Object::Create<ShadowMapObject>("ShadowMap");
		shadowmap->SetCascadeCount(4);
		shadowmap->SetShadowMapSize(2048);
		shadowmap->SetLightDirection({ 0, -8, 5 });
		auto player_ = SceneManager::Object::Create<Player>(u8"プレイヤー");
		player_->transform->scale = { 0.05f,0.05f,0.05f };
		player_->transform->position = { 0,30,100 };

		auto light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
		light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 20,20,100 }, 0, 0, { 0,-8,5 });

		for (u32 i = 0; i < buildings.size(); ++i)
			for (u32 j = 0; j < 20; ++j) {
				Vector3 rand_start = Vector3(-5, 20, -5);
				Vector3 rand_end = Vector3(5, 100, 5);
				Vector3 rand_pos = Random::Position(rand_start, rand_end);
				Vector3 offset = Vector3(rand_pos.x, 0, rand_pos.z);
				rand_pos += offset.getNormalized() * 60;
				rand_pos += buildings[i];
				light_manager->AddLight(LightType::Point, rand_pos, Random::Color({ 0,0,100 }, { 100,100,100 }), 50.0f, 4.0f);

			}

		if constexpr (true) {

			auto ground = SceneManager::Object::Create<GameObject>("Ground");
			ground->AddComponent<ModelRenderer>()->SetModel("stage");
			ground->AddComponent<RigidBody>();
			ground->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);
			ground->transform->scale = { 50,50,50 };
			ground->transform->position = { 0,0,0 };
		}
		camera = SceneManager::Object::Create<CameraObject>();
		camera->transform->position = { 0,10,10 };
		camera->transform->SetAxisZ({ 0,-0.75f,-1.0f });
		camera->camera->render_type = Camera::RenderType::Deferred;
		camera->camera->camera_far = 1000.0f;
		camera->AddComponent<AudioListener>();

		player_->player_camera = camera;
		player = player_;
		for (u32 i = 0; i < buildings.size(); ++i)
		{
			auto building = SceneManager::Object::Create<GameObject>("Building" + std::to_string(i));
			building->AddComponent<ModelRenderer>()->SetModel("building");
			building->transform->scale = { 2.0f,2.0f,2.0f };
			building->transform->position = buildings[i];
			auto rb = building->AddComponent<RigidBody>();
			rb->ChangeToStatic();
			//rb->mass = 0.01f;
			auto collision = building->AddComponent<BoxCollider>();
			collision->extension = { 100.0f, 200.0f, 100.0f };
			collision->position = { 0,100.0f,0 };
			collision->SetLayer(Collider::Layer::Terrain);
		}
		if constexpr (true)
		{
			auto high_way = SceneManager::Object::Create<GameObject>("HighWay");
			auto mod = high_way->AddComponent<ModelRenderer>();
			mod->SetModel("high-way");

			high_way->transform->position = { 0,2,0 };
			high_way->transform->scale = { 3.0f,3.0f,3.0f };
			high_way->AddComponent<RigidBody>();
			high_way->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);

			for (u32 i = 0; i < 5; ++i)
				SceneManager::Object::Create<PoliceCar>(u8"police_car")->SetPointOnpath(100 * i);
		}
		constexpr float gravity_factor = -9.81f * 6;
		GetPhysicsScene()->setGravity({ 0,gravity_factor,0 });

		{
			text_obj = SceneManager::Object::Create<UIObject>(u8"テキストオブジェクト");
			text_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_TOP;
			text_obj->AnchorType() = UIObject::ANCHOR_TYPE::RIGHT_TOP;
			text_obj->transform->scale = { 300,50,1 };
			text_comp = text_obj->AddComponent<Text>();
			text_comp->FontSize() = 50;
			text_comp->TextColor() = Color::MAGENTA;
			text_comp->SetAlignment(Text::ALIGNMENT::RIGHT);
		}
		{
			auto hud_prototype = SceneManager::Object::Create<UIObject>(u8"α版HUD");
			hud_prototype->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_BOTTOM;
			hud_prototype->AnchorType() = UIObject::ANCHOR_TYPE::RIGHT_BOTTOM;
			hud_prototype->transform->scale = { 400, 200, 1 };
			auto hud_text = hud_prototype->AddComponent<Text>();
			hud_text->SetAlignment(Text::ALIGNMENT::RIGHT);
			hud_text->TextColor() = Color::RED;
			//α版以降はチュートリアルも込みで実装予定
			static std::string hud_text_str =
				u8"NeonFade α版 HUD\nカメラ操作:右スティック\n移動:左スティック\nダッシュ(切り替え):左スティック押し込み\nジャンプ:Bボタン\n攻撃(ジャンプ・落下中も可):ZRトリガー\n回避:左スティック+ZLトリガー\n";
			hud_text->SetText(hud_text_str);
		}

		

		scene_state_machine = make_safe_unique<SceneGameStateMachine>(this);
		Time::ResetTime();
		return Super::Init();

	}

	void SceneGame::Update()
	{
		if (!CheckForLoading())
			return;
		scene_state_machine->Update(Time::DeltaTime());


	}

	void SceneGame::PreDraw()
	{

	}

	void SceneGame::LateDebugDraw()
	{

	}
	void SceneGame::OnLateDrawFinish()
	{

		printfDx("%.2f fps\n", Time::GetDrawFPS());
		printfDx("%.2f update_fps\n", Time::GetFPS());
		//DrawBoxAA(0, 50, sample_count_timer * 50.0f, 80, Color::RED, true);

	}

	void SceneGame::Exit()
	{
	}

	bool SceneGame::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADED)
			return true;
		loading_status = ModelManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADED)
			Init();
		return loading_status == LOADING_STATUS::LOADED;
	}

}