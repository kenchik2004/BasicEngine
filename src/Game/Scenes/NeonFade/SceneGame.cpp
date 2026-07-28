// SceneGame.cpp
//---------------------------------------------------------------------------
//! @file   SceneGame.cpp
//! @brief  SceneGameの実装。メインゲームシーンの初期化・更新・描画処理を行う
//---------------------------------------------------------------------------
#include "SceneGame.h"

#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/MiniMapObject.h"
#include "Game/Objects/NeonFade/PoliceCar.h"

#include "Game/Managers/LightManager.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"
#include "Game/Components/PlayerCameraMachine.h"
#include "Game/Objects/DevelopTools/LightEditor.h"

#include "Game/Objects/NeonFade/CountDownTextObject.h"
#include "Game/Objects/NeonFade/TitleObjects/SettingsManager.h"



namespace NeonFade {
	//1ビルの縦横...100m×100m
	std::array<Vector3, 4> buildings = {
		Vector3(-100.0f,0.0f,290.0f),
		Vector3(-100.0f,0.0f,130.0f),
		Vector3(100.0f,0.0f,130.0f),
		Vector3(100.0f,0.0f,290.0f),
	};

	static HANDLE h7seg_font = nullptr;	//!< 7セグのフォントハンドル(Windowsのハンドル。DxLibのハンドルは別物なので注意)

	void SceneGame::LoadVolumeSettings()
	{
		float volume = FileSystem::IniFileManager::Getfloat("Audio", "BGMVolume", 1.0f, "data/config.ini");
		bgm_volume = volume;
		volume = FileSystem::IniFileManager::Getfloat("Audio", "SEVolume", 1.0f, "data/config.ini");
		se_volume = volume;
		if (audio_player)
			audio_player->volume = bgm_volume;
	}

	void SceneGame::LoadCameraSettings()
	{
		if (player) {
			if (player->player_camera_machine)
				player->player_camera_machine->LoadCameraSettings();
		}

	}

	/// @brief ゲーム内で使用するリソースを読み込む
	/// @details モデルやアニメーションなどのデータをメモリに展開する
	void SceneGame::LoadResources()
	{
		// プレイヤー関連のモデルとアニメーションを読み込む
		ModelManager::LoadAsModel(u8"data/player/model_v2.mv1", "player_model");
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
		ModelManager::LoadAsAnimation(u8"data/player/anim_crouch_inv.mv1", "crouch_inv");
		ModelManager::LoadAsAnimation(u8"data/player/anim_spin_kick.mv1", "spin_kick");
		ModelManager::LoadAsAnimation(u8"data/player/anim_leg_sweep.mv1", "leg_sweep");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump_cool.mv1", "jump_cool");
		ModelManager::LoadAsAnimation(u8"data/player/anim_dive.mv1", "player_dive");

		// ステージ関連のモデルを読み込む
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
		ModelManager::LoadAsModel(u8"data/car/police.mv1", "police_car");
		ModelManager::LoadAsModel(u8"data/Stage/crater/crater.mv1", "crater");


		// 敵関連のモデルとアニメーションを読み込む
		ModelManager::LoadAsModel(u8"data/enemy/model LOD.mv1", "enemy_model");
		ModelManager::LoadAsModel(u8"data/enemy/leader_model LOD.mv1", "enemy_leader_model");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_fighting_idle.mv1", "enemy_idle");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_damage.mv1", "enemy_damage");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_down.mv1", "enemy_down");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_down_forward.mv1", "enemy_down_forward");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_standup_back.mv1", "enemy_standup_back");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_standup_forward.mv1", "enemy_standup_forward");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_attack_charge.mv1", "enemy_attack_charge");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_attack_main.mv1", "enemy_attack_main");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_walk.mv1", "enemy_walk");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_escape.mv1", "enemy_escape");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_cover.mv1", "enemy_cover");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_cover_approach.mv1", "enemy_cover_approach");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_crowling.mv1", "enemy_crowling");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_die.mv1", "enemy_die");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_fast_run.mv1", "enemy_fast_run");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_dropkick.mv1", "enemy_dropkick");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_damage_crowling.mv1", "enemy_damage_crowling");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_instruct.mv1", "enemy_instruct");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_pistol_idle.mv1", "enemy_pistol_idle");
		ModelManager::LoadAsAnimation(u8"data/enemy/bl_anim_pistol_grab.mv1", "enemy_pistol_grab");

		// エフェクト用のテクスチャとムービーを読み込む
		TextureManager::Load(u8"data/Textures/FX.png", "fx_texture");
		TextureManager::Load(u8"data/player/thunder.mp4", "electro_movie");
		TextureManager::Load(u8"data/player/Ely By K.Atienza.fbm/ely-vanguardsoldier-kerwinatienza_specular.png", "player_metallic");
		TextureManager::Load(u8"data/player/cutin.png", "cutin_eff");
		TextureManager::Load(u8"data/Textures/MiniMap.png", "mini_map");
		TextureManager::Load(u8"data/Textures/CountDownTextBox.png", "count_down_bg");

		// サウンドとフォントリソースを読み込む
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
		AudioManager::Load(u8"data/sound/enem_crash.mp3", "enem_crash");
		h7seg_font = AddFontFile(u8"data/DSEG7Modern-Regular.ttf");
	}

	/// @brief シーンのロード処理を行う
	/// @details ロード状態の初期化とUIテキストの作成を担う
	void SceneGame::Load()
	{
		// ロード中の状態を設定する
		loading_status = LOADING_STATUS::LOADING;
		LoadResources();

		//ロード中のメッセージテキストを作成
		if (ui_texts.empty()) {
			// UIオブジェクトを生成し中央に配置する
			auto ui_txt_obj = SceneManager::Object::Create<UIObject>(shared_from_this());
			ui_txt_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::CENTER;
			ui_txt_obj->AnchorType() = UIObject::ANCHOR_TYPE::CENTER;
			ui_txt_obj->transform->scale = { 800,170,1 };
			// テキストコンポーネントを追加し見た目を設定する
			auto txt_comp = ui_txt_obj->AddComponent<Text>();
			txt_comp->SetFontSize(170);
			txt_comp->TextColor() = Color::YELLOW;
			txt_comp->SetText(u8"");
			ui_texts["txt_message"] = ui_txt_obj;
			{
				txt_comp->SetText(u8"全員捕まえろ!");
			}

			ui_texts["txt_time"] = SceneManager::Object::Create<CountDownTextObject>(shared_from_this());
		}
		// 初回ロードの確認を行う
		CheckForLoading(false);
	}

	/// @brief ゲームシーンの初期化を行う
	/// @return 初期化の成否を示す整数値
	int SceneGame::Init()
	{
		


		//DontDestroyOnLoadSceneに、カメラ・ライトマネージャー・シャドウマップを作成する
		{
			{
				// 常駐シーンからカメラオブジェクトを取得する
				auto rec_cam = SceneManager::Object::Get<CameraObject>(SceneManager::GetDontDestoryOnLoadScene());
				if (!rec_cam)
					rec_cam = SceneManager::Object::Create<CameraObject>(SceneManager::GetDontDestoryOnLoadScene());

				// カメラの各種パラメータを設定する
				rec_cam->camera->render_type = Camera::RenderType::Deferred;
				rec_cam->camera->clear_type = Camera::ClearType::Color;
				rec_cam->camera->clear_color = { 0,0,1,1 };
				rec_cam->transform->position = { 2,5,0 };
				rec_cam->transform->rotation = Quaternion(DEG2RAD(15), { 1,0,0 }) * Quaternion(DEG2RAD(-15), { 0,1,0 });
				rec_cam->camera->camera_far = 500.0f;
			}
			// 常駐シーンからシャドウマップオブジェクトを取得する
			auto rec_shadowmap = SceneManager::Object::Get<ShadowMapObject>(SceneManager::GetDontDestoryOnLoadScene());
			if (!rec_shadowmap)
				rec_shadowmap = SceneManager::Object::Create<ShadowMapObject>(SceneManager::GetDontDestoryOnLoadScene());

			// シャドウマップの解像度や方向を設定する
			rec_shadowmap->SetCascadeCount(2);
			rec_shadowmap->SetShadowMapSize(1024);
			rec_shadowmap->SetLightDirection({ -5, -8, 5 });

			// 常駐シーンからライトマネージャーを取得し光源を追加する
			auto rec_light_manager = SceneManager::Object::Get<LightManager>(SceneManager::GetDontDestoryOnLoadScene());
			if (!rec_light_manager)
				rec_light_manager = SceneManager::Object::Create<LightManager>(SceneManager::GetDontDestoryOnLoadScene());
			rec_light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 20,20,20 }, 0, 0, { -5,-8,5 });
		}
		// テクスチャのアドレスモードをラップに設定する
		for (int i = 0; i < 6; i++)
			DxLib::SetTextureAddressMode(DX_TEXADDRESS_WRAP, i);
		// メインカメラが存在しない場合は作成する
		if (!camera) {
			camera = SceneManager::Object::Create<CameraObject>();


		}



		// シャドウマップが存在しない場合は作成する
		if (!shadowmap) {
			shadowmap = SceneManager::Object::Create<ShadowMapObject>("ShadowMap");
			shadowmap->SetCascadeCount(4);
			shadowmap->SetShadowMapSize(2048);
			shadowmap->SetLightDirection({ 5, -10, -8 });
		}

		// ライトマネージャーが存在しない場合は作成する
		if (!light_manager) {

			light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
			light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 15,8,10 }, 0, 0, { 5,-10,-8 });

			{
				// 特殊エフェクト用のマテリアルを作成する
				auto mat = MaterialManager::CreateMaterial("hunt_eff_mat");
				auto tex = SceneManager::GetDontDestoryOnLoadScene()->GetCurrentCamera()->hdr;
				auto depth = SceneManager::GetDontDestoryOnLoadScene()->GetCurrentCamera()->depth;
				mat->SetTexture(tex, Material::TextureType::Diffuse);
				mat->SetTexture(depth, Material::TextureType::Normal);
				auto movie = TextureManager::Get("cutin_eff");
				mat->SetTexture(movie, Material::TextureType::Emission);
				// シェーダーを割り当てる
				mat->SetShaderPs(MaterialManager::LoadPixelShader(u8"data/shader/ps_hunted_effect.fx", u8"ps_hunted_effect"));
				mat->SetShaderVs(MaterialManager::GetDefaultMat2D()->GetVertexShader());
			}
		}
		// ライトエディタが存在しない場合は作成してデータを読み込む
		if (!light_editor) {
			light_editor = SceneManager::Object::Create<LightEditor>();
			light_editor->file_path = "data/SceneGame_LightData.txt";
		}
		light_editor->Load(light_editor->file_path);

		// ロードが未完了の場合は処理を中断する
		if (!CheckForLoading())
			return 0;
		// オーディオプレイヤーが存在しない場合は作成する
		if (!audio_player) {
			auto audio_player_obj = SceneManager::Object::Create<GameObject>();
			audio_player = audio_player_obj->AddComponent<AudioPlayer>();
			audio_player->is_3d = false;
			// 設定ファイルから音量を読み込む
			LoadVolumeSettings();
		}
		// プレイヤーが存在しない場合は作成する
		if (!player) {
			auto player_ = SceneManager::Object::Create<Player>(u8"プレイヤー");

			// プレイヤーの初期位置を設定する
			player_->transform->position = { 0,20,100 };



			{
				// 地面オブジェクトを作成する
				auto ground = SceneManager::Object::Create<GameObject>("Ground");
				auto ground_mdl = ground->AddComponent<ModelRenderer>();
				ground_mdl->SetModel("stage");
				ground->AddComponent<RigidBody>();
				ground->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);
				ground->transform->scale = { 10,10,10 };
				ground->transform->position = { 0,0,200 };

			}
			{
				// カメラの初期設定とプレイヤー追従機能を追加する
				camera->transform->position = { 0,10,10 };
				camera->transform->SetAxisZ({ 0,-0.75f,-1.0f });
				camera->camera->render_type = Camera::RenderType::Deferred;
				camera->camera->camera_far = 3000.0f;
				camera->camera->perspective = 50.0f;
				camera->AddComponent<AudioListener>();
				auto machine = camera->AddComponent<PlayerCameraMachine>();

				player_->player_camera = camera;
				player_->player_camera_machine = machine;
				machine->SetTarget(player_);
			}

			player = player_;
			SceneManager::Object::Create<MiniMapObject>(shared_from_this());
		}
		// ビル群を配置する
		for (u32 i = 0; i < buildings.size(); ++i)
		{
			auto building = SceneManager::Object::Create<GameObject>("Building" + std::to_string(i));
			building->AddComponent<ModelRenderer>()->SetModel("building");
			building->transform->scale = { 2.0f,2.0f,2.0f };
			building->transform->position = buildings[i];
			// 物理挙動を設定し静的オブジェクトにする
			auto rb = building->AddComponent<RigidBody>();
			rb->ChangeToStatic();
			auto collision = building->AddComponent<BoxCollider>();
			collision->extension = { 100.0f, 200.0f, 100.0f };
			collision->position = { 0,100.0f,0 };
			collision->SetLayer(Collider::Layer::Terrain);
		}
		{
			// 背景用のタワーモデルを配置する
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
			// 空中のハイウェイモデルを配置する
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
			// 地上のハイウェイモデルを配置する
			auto high_way = SceneManager::Object::Create<GameObject>("HighWay");
			auto mod = high_way->AddComponent<ModelRenderer>();
			mod->SetModel("high-way");
			high_way->transform->scale = { 3.0f,3.0f,3.0f };
			high_way->AddComponent<RigidBody>();
			high_way->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);

		}
		// 街灯モデルを一定間隔で配置する
		for (int i = 0; i < 5; ++i) {
			auto street_light_prototype = SceneManager::Object::Create<GameObject>("StreetLightPrototype");
			auto mod = street_light_prototype->AddComponent<ModelRenderer>();
			mod->SetModel("street_light");
			street_light_prototype->transform->scale = { 0.05f,0.05f,0.05f };
			street_light_prototype->transform->position = { 50 - i * 10.0f,0,200 };

		}
		// 物理シーンの重力を設定する
		constexpr float gravity_factor = -9.81f * 6;
		GetPhysicsScene()->setGravity({ 0,gravity_factor,0 });
		// 汎用テキストUIが存在しない場合は作成する
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
			// リリースビルド時はテキストを非表示にする
			text_comp->Sleep();
#endif
		}
		// HUDが存在しない場合は作成する
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
				u8"NeonFade  HUD\nカメラ操作:右スティック\n移動:左スティック\nダッシュ(切り替え):左スティック押し込み\nジャンプ:Bボタン\n攻撃(ジャンプ・落下中も可):AXYどれか\n回避:左スティック+ZLトリガー\nスタートボタンを押してポーズ";
			hud_text->SetText(hud_text_str);
			hud_obj = hud_prototype;


		}


		// ステートマシンが存在しない場合は作成する
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
			// 見えない壁のコライダーを配置する
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

		{
			bool polis_car_enabled = FileSystem::IniFileManager::GetBool("Game", "police_car_enabled", false, "data/config.ini");
			if (polis_car_enabled) {
				auto police_car = SceneManager::Object::Create<PoliceCar>(u8"パトカー");
			}
			pause_menu_obj = SceneManager::Object::Create<SettingsManager>(u8"ポーズメニュー");
		}
		// 親クラスの初期化関数を呼び出す
		return Super::Init();

	}

	/// @brief フレーム毎の更新処理を行う
	/// @details ロード完了後にステートマシンとタイマーを更新しポーズ操作を受け付ける
	void SceneGame::Update()
	{
		// ロードが未完了の場合は処理をスキップする
		if (!CheckForLoading())
			return;

		// ステートマシンを更新する
		scene_state_machine->Update(Time::DeltaTime());

		// ポーズ可能な状態で対応ボタンが押されたらポーズ状態を切り替える
		if ((Input::GetPadButtonDown(0, PadButton::Start) || Input::GetKeyDown(KeyCode::Minus))
			&& is_pause_available) {
			//hud_obj->GetComponent<Text>()->Sleep();
			PauseGame(!is_game_paused);
			EnablePauseMenu(is_game_paused);
		}
		// タイマーが開始されていれば時間を進める
		if (is_game_timer_started)
			game_timer += Time::UnscaledDeltaTime();




	}

	/// @brief 描画前の準備処理を行う
	void SceneGame::PreDraw()
	{
	}

	/// @brief 遅延デバッグ描画処理を行う
	void SceneGame::LateDebugDraw()
	{

	}
	/// @brief 遅延描画処理を行う
	/// @details ライティングを無効化してデバッグ描画を行い再度有効化する
	void SceneGame::LateDraw()
	{
		// ライティングを一時的に無効化する
		SetUseLighting(false);
		if (light_manager && Input::GetKey(KeyCode::Alpha9))
			light_manager->LateDebugDraw();
		// ライティングを有効化する
		SetUseLighting(true);
	}
	/// @brief 遅延描画完了時のイベント処理を行う
	/// @details ロード中であればアニメーションするテキストを表示する
	void SceneGame::OnLateDrawFinish()
	{

		if (!CheckForLoading())
		{
			// 経過時間からドットの数を計算する
			float cnt = Time::GetTimeFromStart();
			int dot_cnt = int(cnt * 2) % 4;
			std::string load_txt = u8"ロード中";
			// ドットを付加する
			for (int i = 0; i < dot_cnt; ++i)
				load_txt += '.';
			ui_texts["txt_message"]->GetComponent<Text>()->SetText(load_txt);
		}
	}

	/// @brief シーンの終了処理を行う
	/// @details 使用したリソースの解放などを行う
	void SceneGame::Exit()
	{
		// フォントリソースを削除する
		RemoveFontFile(h7seg_font);
		// 親クラスの終了関数を呼び出す
		Super::Exit();
	}

	/// @brief リソースのロード状態を確認する
	/// @param init ロード完了時に初期化処理を呼び出すかどうかのフラグ
	/// @return ロードが完了していればtrueを返す
	bool SceneGame::CheckForLoading(bool init)
	{
		// 既にロード完了状態であればtrueを返す
		if (loading_status == LOADING_STATUS::LOADED)
			return true;
		// 各マネージャーのロード残数を確認して状態を更新する
		loading_status = (ModelManager::GetLoadingCount() + TextureManager::GetLoadingCount() + AudioManager::GetLoadingCount()) == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		// ロード完了かつ初期化フラグが立っていれば初期化処理を呼ぶ
		if (loading_status == LOADING_STATUS::LOADED && init)
			Init();
		return loading_status == LOADING_STATUS::LOADED;
	}

	/// @brief ゲームのポーズ状態を切り替える
	/// @param pause ポーズする場合はtrueを指定する
	void SceneGame::PauseGame(bool pause)
	{
		is_game_paused = pause;
		if (pause) {
			// タイマーを停止させる
			StopGameTimer();
			//プレイヤーと敵を一時停止
			//描画は有効
			// プレイヤーの動作を停止させる
			player->Sleep(false);
			auto enemies = Enemy::GetAllEnemies();
			// 全ての敵の動作を停止させる
			for (auto& enem : enemies)
				enem->Sleep(false);
		}
		else {
			// タイマーを再開させる
			StartGameTimer();
			// プレイヤーの動作を再開させる
			player->WakeUp();
			auto enemies = Enemy::GetAllEnemies();
			// 全ての敵の動作を再開させる
			for (auto& enem : enemies)
				enem->WakeUp();
			return;
		}



	}

	void SceneGame::EnablePauseMenu(bool enable)
	{
		if (pause_menu_obj)
		{
			if (enable)
				pause_menu_obj->Open();
			else
				pause_menu_obj->Close();
		}
	}

	/// @brief 現在の敵の数を減算する
	/// @param cnt 減算する数
	void SceneGame::SubtractEnemyCount(u32 cnt)
	{
		// 敵の数が0未満にならないように減算する
		enemy_count -= min(cnt, enemy_count);
	}

	/// @brief 全ての敵オブジェクトを削除しカウントを0にする
	void SceneGame::ClearAllEnemy()
	{
		// 敵カウントをリセットする
		enemy_count = 0;
		auto enems = SceneManager::Object::GetArray<Enemy>();
		// 全ての敵オブジェクトを破棄する
		for (auto& enem : enems)
			SceneManager::Object::Destroy(enem);

	}



}