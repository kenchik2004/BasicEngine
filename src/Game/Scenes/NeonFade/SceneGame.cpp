//---------------------------------------------------------------------------
//! @file   SceneGame.cpp
//! @brief  SceneGameの実装。メインゲームシーンの初期化・更新・描画処理を行う
//---------------------------------------------------------------------------
#include "SceneGame.h"

#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/PoliceCar.h"

#include "Game/Managers/LightManager.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"
#include "Game/Components/PlayerCameraMachine.h"
#include "Game/Objects/DevelopTools/LightEditor.h"



namespace NeonFade {
	//1ビルの縦横...100m×100m
	std::array<Vector3, 4> buildings = {
		Vector3(-100.0f,0.0f,290.0f),
		Vector3(-100.0f,0.0f,130.0f),
		Vector3(100.0f,0.0f,130.0f),
		Vector3(100.0f,0.0f,290.0f),
	};
	SafeSharedPtr<LightEditor> light_editor = nullptr; //!< ライトエディタオブジェクト

	void SceneGame::LoadResources()
	{

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
		ModelManager::LoadAsAnimation(u8"data/player/anim_spin.mv1", "spin");
		ModelManager::LoadAsAnimation(u8"data/player/anim_clouch_inv.mv1", "clouch_inv");
		ModelManager::LoadAsAnimation(u8"data/player/anim_spin_kick.mv1", "spin_kick");
		ModelManager::LoadAsAnimation(u8"data/player/anim_leg_sweep.mv1", "leg_sweep");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump_cool.mv1", "jump_cool");

		ModelManager::LoadAsModel(u8"data/Stage/Buildings/Ground.mv1", "stage");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/building-01_UV.mv1", "building");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower1.mv1", "sky_tower1");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower2.mv1", "sky_tower2");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower3.mv1", "sky_tower3");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower4.mv1", "sky_tower4");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower5.mv1", "sky_tower5");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower6.mv1", "sky_tower6");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower7.mv1", "sky_tower7");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower8.mv1", "sky_tower8");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower9.mv1", "sky_tower9");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower10.mv1", "sky_tower10");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower11.mv1", "sky_tower11");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower12.mv1", "sky_tower12");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower13.mv1", "sky_tower13");
		ModelManager::LoadAsModel(u8"data/Stage/megapolis/road.mv1", "high-way");
		ModelManager::LoadAsModel(u8"data/Stage/megapolis/road_straight_lod.mv1", "sky_highway");
		ModelManager::LoadAsModel(u8"data/Stage/light/street_light.mv1", "street_light");

		ModelManager::LoadAsModel(u8"data/enemy/X Bot.mv1", "enemy_model");
		ModelManager::LoadAsModel(u8"data/enemy/X Bot_LOD.mv1", "enemy_model_LOD");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_fighting_idle.mv1", "enemy_idle");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_damage.mv1", "enemy_damage");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_t_pose.mv1", "enemy_die");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_attack_charge.mv1", "enemy_attack_charge");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_attack_main.mv1", "enemy_attack_main");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_walk.mv1", "enemy_walk");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_escaping.mv1", "enemy_escape");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_down.mv1", "enemy_down");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_down_forward.mv1", "enemy_down_forward");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_instructing.mv1", "enemy_instruct");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_stepback.mv1", "enemy_stepback");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_hunted.mv1", "enemy_hunted");

		TextureManager::Load(u8"data/FX.png", "fx_texture");
		TextureManager::Load(u8"data/player/thunder.mp4", "electro_movie");
		TextureManager::Load(u8"data/player/cutin.png", "cutin_eff");

		AudioManager::Load(u8"data/sound/bgm.mp3", "bgm");
		AudioManager::Load(u8"data/sound/hit_se.mp3", "hit_se");
		AudioManager::Load(u8"data/sound/smash_se.mp3", "smash_se");
		AudioManager::Load(u8"data/sound/smash_charge_se.mp3", "smash_charge_se");
		AudioManager::Load(u8"data/sound/knockout_se.mp3", "knockout_se");
		AudioManager::Load(u8"data/sound/hunt_se.mp3", "hunt_se");
		AudioManager::Load(u8"data/sound/assert_se.mp3", "assert_se");
		AudioManager::Load(u8"data/sound/finish_se.mp3", "finish_se");
		AudioManager::Load(u8"data/sound/result_bgm.mp3", "result_bgm");
		AudioManager::Load(u8"data/sound/score_se.mp3", "score_se");
		AudioManager::Load(u8"data/sound/welter_se.mp3", "welter_se");
	}


	void SceneGame::Load()
	{

		loading_status = LOADING_STATUS::LOADING;
		LoadResources();

		//ロード中のメッセージテキストを作成
		if (ui_texts.empty()) {
			std::array<std::string, 2> ui_name_table = { "txt_message","txt_time" };
			std::array<std::string, 2> ui_txt_table = { u8"",u8"" };
			for (u32 i = 0; i < ui_name_table.size(); i++) {
				auto txt_obj = SceneManager::Object::Create<UIObject>(shared_from_this());
				txt_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::CENTER;
				txt_obj->AnchorType() = UIObject::ANCHOR_TYPE::CENTER;
				txt_obj->transform->scale = { 300,170,1 };
				auto txt_comp = txt_obj->AddComponent<Text>();
				txt_comp->SetFontSize(170);
				txt_comp->TextColor() = Color::YELLOW;
				txt_comp->SetAlignment(Text::ALIGNMENT::MIDDLE);
				txt_comp->SetText(ui_txt_table[i]);
				ui_texts[ui_name_table[i]] = txt_obj;

			}
			ui_texts["txt_time"]->CanvasAnchorType() = UIObject::ANCHOR_TYPE::CENTER_TOP;
			ui_texts["txt_time"]->AnchorType() = UIObject::ANCHOR_TYPE::CENTER_TOP;
			ui_texts["txt_time"]->GetComponent<Text>()->SetFontSize(80);
		}
		CheckForLoading(false);
	}

	int SceneGame::Init()
	{
		for (int i = 0; i < 6; i++)
			DxLib::SetTextureAddressMode(DX_TEXADDRESS_WRAP, i);
		if (!camera) {
			camera = SceneManager::Object::Create<CameraObject>();

			auto rec_cam = SceneManager::Object::Create<CameraObject>(SceneManager::GetDontDestoryOnLoadScene());
			rec_cam->camera->render_type = Camera::RenderType::Deferred;
			rec_cam->camera->clear_type = Camera::ClearType::Color;
			rec_cam->camera->clear_color = { 0,0,1,1 };
			rec_cam->transform->position = { 2,5,0 };
			rec_cam->transform->rotation = Quaternion(DEG2RAD(15), { 1,0,0 }) * Quaternion(DEG2RAD(-15), { 0,1,0 });
			rec_cam->camera->camera_far = 500.0f;
		}



		if (!shadowmap) {
			shadowmap = SceneManager::Object::Create<ShadowMapObject>("ShadowMap");
			shadowmap->SetCascadeCount(4);
			shadowmap->SetShadowMapSize(2048);
			shadowmap->SetLightDirection({ 5, -10, -8 });
			auto rec_shadowmap = SceneManager::Object::Create<ShadowMapObject>(SceneManager::GetDontDestoryOnLoadScene());
			rec_shadowmap->SetCascadeCount(2);
			rec_shadowmap->SetShadowMapSize(1024);
			rec_shadowmap->SetLightDirection({ -5, -8, 5 });
		}

		if (!light_manager) {

			light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
			light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 15,8,10 }, 0, 0, { 5,-10,-8 });
			auto rec_light_manager = SceneManager::Object::Create<LightManager>(SceneManager::GetDontDestoryOnLoadScene());
			rec_light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 20,20,20 }, 0, 0, { -5,-8,5 });
			for (u32 i = 0; i < buildings.size(); ++i)
				for (u32 j = 0; j < 20; ++j) {
					Vector3 rand_start = Vector3(-5, 20, -5);
					Vector3 rand_end = Vector3(5, 100, 5);
					Vector3 rand_pos = Random::Position(rand_start, rand_end);
					Vector3 offset = Vector3(rand_pos.x, 0, rand_pos.z);
					rand_pos += offset.getNormalized() * 60;
					rand_pos += buildings[i];
					light_manager->AddLight(LightType::Point, rand_pos, Random::Color({ 100,100,500 }, { 1000,1000,1000 }), Random::Range(10.0f, 50.0f), Random::Range(0.01f, 10.0f));

				}

			{

				auto mat = MaterialManager::CreateMaterial("hunt_eff_mat");
				auto tex = SceneManager::GetDontDestoryOnLoadScene()->GetCurrentCamera()->hdr;
				auto depth = SceneManager::GetDontDestoryOnLoadScene()->GetCurrentCamera()->depth;
				mat->SetTexture(tex, Material::TextureType::Diffuse);
				mat->SetTexture(depth, Material::TextureType::Normal);
				auto movie = TextureManager::Get("cutin_eff");
				mat->SetTexture(movie, Material::TextureType::Emission);
				mat->SetShaderPs(MaterialManager::LoadPixelShader(u8"data/shader/ps_hunted_effect.fx", u8"ps_hunted_effect"));
				mat->SetShaderVs(MaterialManager::GetDefaultMat2D()->GetVertexShader());
			}
		}
		if (!light_editor) {
			light_editor = SceneManager::Object::Create<LightEditor>();
			light_editor->file_path = "data/SceneGame_LightData.txt";
		}

		if (!CheckForLoading())
			return 0;
		if (!audio_player) {
			auto audio_player_obj = SceneManager::Object::Create<GameObject>();
			audio_player = audio_player_obj->AddComponent<AudioPlayer>();
			audio_player->is_3d = false;
		}
		if (!player) {
			auto player_ = SceneManager::Object::Create<Player>(u8"プレイヤー");

			player_->transform->position = { 0,20,100 };



			{
				auto ground = SceneManager::Object::Create<GameObject>("Ground");
				auto ground_mdl = ground->AddComponent<ModelRenderer>();
				ground_mdl->SetModel("stage");
				ground->AddComponent<RigidBody>();
				ground->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);
				ground->transform->scale = { 10,10,10 };
				ground->transform->position = { 0,0,200 };

			}
			{
				camera->transform->position = { 0,10,10 };
				camera->transform->SetAxisZ({ 0,-0.75f,-1.0f });
				camera->camera->render_type = Camera::RenderType::Deferred;
				camera->camera->camera_far = 3000.0f;
				camera->AddComponent<AudioListener>();
				auto machine = camera->AddComponent<PlayerCameraMachine>();

				player_->player_camera = camera;
				player_->player_camera_machine = machine;
				machine->SetTarget(player_);
			}

			player = player_;
		}
		for (u32 i = 0; i < buildings.size(); ++i)
		{
			auto building = SceneManager::Object::Create<GameObject>("Building" + std::to_string(i));
			building->AddComponent<ModelRenderer>()->SetModel("building");
			building->transform->scale = { 2.0f,2.0f,2.0f };
			building->transform->position = buildings[i];
			auto rb = building->AddComponent<RigidBody>();
			rb->ChangeToStatic();
			auto collision = building->AddComponent<BoxCollider>();
			collision->extension = { 100.0f, 200.0f, 100.0f };
			collision->position = { 0,100.0f,0 };
			collision->SetLayer(Collider::Layer::Terrain);
		}
		{
			static constexpr int SKY_TOWER_COUNT = 12;
			std::array<std::string, SKY_TOWER_COUNT> sky_tower_model_names = {
				"sky_tower1",
				"sky_tower2",
				"sky_tower3",
				"sky_tower4",
				"sky_tower5",
				"sky_tower6",
				"sky_tower7",
				"sky_tower8",
				"sky_tower9",
				"sky_tower12",
				"sky_tower13",
				"sky_tower13",
			};
			std::array<Vector3, SKY_TOWER_COUNT> sky_tower_positions = {
				Vector3(-1070,0,-480),
				Vector3(0,0,-940),
				Vector3(0,0,1910),
				Vector3(1020,0,870),
				Vector3(-1010,0,1110),
				Vector3(-1800,0,230),
				Vector3(1140,0,-730),
				Vector3(680,0,1600),
				Vector3(-1310,0,-1020),
				Vector3(1620,0,-20),
				Vector3(-340,0,1430),
				Vector3(650,0,-707)
			};
			std::array<Quaternion, SKY_TOWER_COUNT> sky_tower_rotations = {
				Quaternion(0.898f, { 0,1,0 }),
				Quaternion(0.0f, { 0,1,0 }),
				Quaternion(0.0f, { 0,1,0 }),
				Quaternion(1.436f, { 0,1,0 }),
				Quaternion(5.745f, { 0,1,0 }),
				Quaternion(1.571f, { 0,1,0 }),
				Quaternion(1.661f, { 0,1,0 }),
				Quaternion(0.0f, { 0,1,0 }),
				Quaternion(0.0f, { 0,1,0 }),
				Quaternion(4.712f, { 0,1,0 }),
				Quaternion(5.61f, { 0,1,0 }),
				Quaternion(6.104f, { 0,1,0 })
			};
			std::array<Vector3, SKY_TOWER_COUNT> sky_tower_scales = {
				Vector3(1.8f,1.8f,1.8f),
				Vector3(2.7f,2.7f,2.7f),
				Vector3(2.5f,2.5f,2.5f),
				Vector3(3.5f,3.5f,3.5f),
				Vector3(2.0f,2.0f,2.0f),
				Vector3(1.5f,1.5f,1.5f),
				Vector3(2.8f,2.8f,2.8f),
				Vector3(2.6f,2.6f,2.6f),
				Vector3(3.4f,9.9f,3.4f),
				Vector3(3.1f,3.1f,3.1f),
				Vector3(1.2f,1.2f,1.2f),
				Vector3(1.2f,1.2f,1.2f)
			};
			for (int i = 0; i < SKY_TOWER_COUNT; ++i)
			{
				auto sky_tower = SceneManager::Object::Create<GameObject>("SkyTower" + std::to_string(i + 1));
				auto mod = sky_tower->AddComponent<ModelRenderer>();
				mod->SetModel(sky_tower_model_names[i]);
				sky_tower->transform->position = sky_tower_positions[i];
				sky_tower->transform->rotation = sky_tower_rotations[i];
				sky_tower->transform->scale = sky_tower_scales[i];
			}
		}
		{
			static constexpr int SKY_HIGHWAY_COUNT = 10;
			std::array<Vector3, SKY_HIGHWAY_COUNT> sky_highway_positions = {
			Vector3(-381,563,1474),
			Vector3(-199,563,1245),
			Vector3(-25,563,1017),
			Vector3(130,563,777),
			Vector3(271,563,521),
			Vector3(377,563,254),
			Vector3(461,563,-25),
			Vector3(540,563,-310),
			Vector3(612,563,-593),
			Vector3(678,563,-881),

			};
			std::array<Quaternion, SKY_HIGHWAY_COUNT> sky_highway_rotations = {
			Quaternion(0.898f, { 0,1,0 }),
			Quaternion(0.898f, { 0,1,0 }),
			Quaternion(0.942f, { 0,1,0 }),
			Quaternion(1.032f, { 0,1,0 }),
			Quaternion(1.122f, { 0,1,0 }),
			Quaternion(1.257f, { 0,1,0 }),
			Quaternion(1.302f, { 0,1,0 }),
			Quaternion(1.302f, { 0,1,0 }),
			Quaternion(1.346f, { 0,1,0 }),
			Quaternion(1.346f, { 0,1,0 }),

			};
			Vector3 sky_highway_scale = { 0.1f,0.1f,0.1f };

			for (int i = 0; i < SKY_HIGHWAY_COUNT; ++i) {
				auto sky_highway = SceneManager::Object::Create<GameObject>("SkyHighway" + std::to_string(i + 1));
				auto mod = sky_highway->AddComponent<ModelRenderer>();
				mod->SetModel("sky_highway");
				sky_highway->transform->position = sky_highway_positions[i];
				sky_highway->transform->rotation = sky_highway_rotations[i];
				sky_highway->transform->scale = sky_highway_scale;
			}
		}
		{
			auto high_way = SceneManager::Object::Create<GameObject>("HighWay");
			auto mod = high_way->AddComponent<ModelRenderer>();
			mod->SetModel("high-way");

			high_way->transform->scale = { 3.0f,3.0f,3.0f };
			high_way->AddComponent<RigidBody>();
			high_way->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);

		}
		for (int i = 0; i < 5; ++i) {
			auto street_light_prototype = SceneManager::Object::Create<GameObject>("StreetLightPrototype");
			auto mod = street_light_prototype->AddComponent<ModelRenderer>();
			mod->SetModel("street_light");
			street_light_prototype->transform->scale = { 0.05f,0.05f,0.05f };
			street_light_prototype->transform->position = { 50 - i * 10.0f,0,200 };

		}
		constexpr float gravity_factor = -9.81f * 6;
		GetPhysicsScene()->setGravity({ 0,gravity_factor,0 });
		if (!text_obj)
		{
			text_obj = SceneManager::Object::Create<UIObject>(u8"TextObj");
			text_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_TOP;
			text_obj->AnchorType() = UIObject::ANCHOR_TYPE::RIGHT_TOP;
			text_obj->transform->scale = { 300,50,1 };
			text_comp = text_obj->AddComponent<Text>();
			text_comp->SetFontSize(50);
			text_comp->TextColor() = Color::MAGENTA;
			text_comp->SetAlignment(Text::ALIGNMENT::RIGHT);
#ifndef _DEBUG
			text_comp->Sleep();
#endif
		}
		if (!hud_obj)
		{
			auto hud_prototype = SceneManager::Object::Create<UIObject>(u8"HUD");
			hud_prototype->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_BOTTOM;
			hud_prototype->AnchorType() = UIObject::ANCHOR_TYPE::RIGHT_BOTTOM;
			hud_prototype->transform->scale = { 500, 200, 1 };
			auto hud_text = hud_prototype->AddComponent<Text>();
			hud_text->SetAlignment(Text::ALIGNMENT::RIGHT);
			hud_text->TextColor() = Color::RED;
			static std::string hud_text_str =
				u8"NeonFade  HUD\nカメラ操作:右スティック\n移動:左スティック\nダッシュ(切り替え):左スティック押し込み\nジャンプ:Bボタン\n攻撃(ジャンプ・落下中も可):ABXYどれか+ZRトリガー\n回避:左スティック+ZLトリガー\nスタートボタンを押してこのHUDを閉じる";
			hud_text->SetText(hud_text_str);
			hud_obj = hud_prototype;
			ui_texts["txt_message"]->GetComponent<Text>()->SetText(u8"全員捕まえろ!");
		}


		if (!scene_state_machine)
			scene_state_machine = make_safe_unique<SceneGameStateMachine>(this);
		Time::ResetTime();

		//今の状態では、敵やプレイヤーが非常に広範囲に移動できてしまうので、
		//見えない壁を用意
		{
			std::array<Vector3, 4> wall_positions = {
				Vector3(0,20,650),
				Vector3(0,20,-250),
				Vector3(-300,20,200),
				Vector3(300,20,200)
			};
			std::array<Vector3, 4> wall_extents = {
				Vector3(600.0f,100.0f,20.0f),
				Vector3(600.0f,100.0f,20.0f),
				Vector3(20.0f,100.0f,900.0f),
				Vector3(20.0f,100.0f,900.0f)
			};
			for (u32 i = 0; i < wall_positions.size(); ++i) {
				auto wall = SceneManager::Object::Create<GameObject>("Wall" + std::to_string(i));
				wall->transform->position = wall_positions[i];
				auto rb = wall->AddComponent<RigidBody>();
				rb->is_kinematic = true;
				auto col = wall->AddComponent<BoxCollider>();
				col->extension = wall_extents[i];
				col->SetLayer(Collider::Layer::Terrain);
			}



		}
		return Super::Init();

	}

	void SceneGame::Update()
	{
		if (!CheckForLoading())
			return;

		scene_state_machine->Update(Time::DeltaTime());
		if (Input::GetPadButtonDown(0, PadButton::Start) || Input::GetKeyDown(KeyCode::Minus)) {
			hud_obj->GetComponent<Text>()->Sleep();

		}
		if (is_game_timer_started)
			game_timer += Time::UnscaledDeltaTime();
		int min_ = static_cast<int>(max(0.0f, GAME_TIMER_MAX - game_timer)) / 60;
		float sec_ = max(0.0f, GAME_TIMER_MAX - game_timer) - min_ * 60;
		std::string count_down_txt;
		count_down_txt += u8"残り時間 ";
		count_down_txt += std::format("{:02d}", min_);
		count_down_txt += u8":";
		count_down_txt += std::format("{:05.2f}", sec_);

		ui_texts["txt_time"]->GetComponent<Text>()->SetText(count_down_txt);
		if (min_ < 1 && sec_ < 10.0f) {
			ui_texts["txt_time"]->GetComponent<Text>()->TextColor() = Color::RED;
			if (sec_ < 5.0f) {
				ui_texts["txt_time"]->GetComponent<Text>()->TextColor().a = sinf(Time::GetTimeFromStart() * 5) * 0.5f + 1.0f;
				ui_texts["txt_time"]->GetComponent<Text>()->SetFontSize(80 + static_cast<int>(10 * sinf(Time::GetTimeFromStart() * 5)));
			}
		}
		else {
			ui_texts["txt_time"]->GetComponent<Text>()->TextColor() = Color::YELLOW;
		}


	}

	void SceneGame::PreDraw()
	{}

	void SceneGame::LateDebugDraw()
	{

	}
	void SceneGame::LateDraw()
	{
		SetUseLighting(false);
		if (light_manager && Input::GetKey(KeyCode::Alpha9))
			light_manager->LateDebugDraw();
		SetUseLighting(true);
	}
	void SceneGame::OnLateDrawFinish()
	{

		printfDx("%.2f fps\n", Time::GetDrawFPS());
		printfDx("%.2f update_fps\n", Time::GetFPS());
		if (!CheckForLoading())
		{
			float cnt = Time::GetTimeFromStart();
			int dot_cnt = int(cnt * 2) % 4;
			std::string load_txt = u8"ロード中";
			for (int i = 0; i < dot_cnt; ++i)
				load_txt += '.';
			ui_texts["txt_message"]->GetComponent<Text>()->SetText(load_txt);
		}
	}

	void SceneGame::Exit()
	{}

	bool SceneGame::CheckForLoading(bool init)
	{
		if (loading_status == LOADING_STATUS::LOADED)
			return true;
		loading_status = (ModelManager::GetLoadingCount() + TextureManager::GetLoadingCount() + AudioManager::GetLoadingCount()) == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADED && init)
			Init();
		return loading_status == LOADING_STATUS::LOADED;
	}

	void SceneGame::SubtractEnemyCount(u32 cnt)
	{
		enemy_count -= min(cnt, enemy_count);
	}

	void SceneGame::ClearAllEnemy()
	{
		enemy_count = 0;
		auto enems = SceneManager::Object::GetArray<Enemy>();
		for (auto& enem : enems)
			SceneManager::Object::Destroy(enem);

	}



}