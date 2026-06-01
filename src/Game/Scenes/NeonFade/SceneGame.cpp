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
#include "Game/Objects/NeonFade/HuntedEffect.h"


namespace NeonFade {
	class HuntedEffectCreater {
	private:
		float regist_timer = -0.1f;
		static constexpr float REGIST_INTERVAL = 0.3f;
		std::vector<GameObjectP> effect_enemys;
		bool regist_available = true;

		float effect_duration_timer = 0.0f;
		static constexpr float EFFECT_DURATION = 2.0f;
		UIObjectWP effect_obj;
		UIObjectWP effect_text_obj;
		SafeWeakPtr<AudioClip> hunt_se;
	public:
		HuntedEffectCreater() {
			hunt_se = AudioManager::CloneByName(u8"hunt_se");
		}
		bool IsEffectActive() const { return effect_obj.lock() != nullptr; }
		bool IsEffectPreparing() const { return effect_enemys.size() > 0; }
		void Update() {
			regist_timer -= Time::UnscaledDeltaTime();
			if (!IsEffectActive() && effect_enemys.size() > 0 && regist_timer < 0.0f)
				CreateEffect();
			effect_duration_timer -= Time::UnscaledDeltaTime();
			if (IsEffectActive() && effect_duration_timer < 0.0f) {
				ResetEffect();
			}
		}
		void RegistEnemy() {
			if (!regist_available && effect_duration_timer > 0.0f)
				return;
			if (!regist_available)
				regist_available = true;
			auto scene = SceneManager::GetDontDestoryOnLoadScene();
			auto enem = SceneManager::Object::Create<GameObject>(scene);

			enem->AddComponent<ModelRenderer>()->SetModel("enemy_model_LOD");
			Vector3 rand_pos = Random::Position({ -3,0,-3 }, { 3,0,3 });
			enem->transform->position = Vector3(-2, -5, 20) + rand_pos;
			enem->transform->scale = { 0.05f,0.05f,0.05f };
			auto anim = enem->AddComponent<Animator>();
			anim->SetAnimation("enemy_hunted", 0);
			anim->Play("enemy_hunted", true, Random::Float01() * 0.2f);
			effect_enemys.push_back(enem);
			ResetInterval();
		}
		void ClearAll() {
			auto scene = SceneManager::GetDontDestoryOnLoadScene();
			for (auto enem = effect_enemys.begin(); enem != effect_enemys.end();) {
				auto enem_ptr = *enem;
				SceneManager::Object::Destroy(enem_ptr->GetScene(), enem_ptr);
				enem = effect_enemys.erase(enem);
			}
		}
		void ResetInterval() {
			regist_timer = REGIST_INTERVAL;
		}
		void ResetEffect() {
			if (effect_obj)
				SceneManager::Object::Destroy(effect_obj.lock());
			if (effect_text_obj)
				SceneManager::Object::Destroy(effect_text_obj.lock());
			ClearAll();
			effect_obj.reset();
			effect_text_obj.reset();
		}
		void CreateEffect() {
			regist_available = false;
			auto effect_scene = SceneManager::GetCurrentScene();
			if (!effect_obj) {
				effect_obj = SceneManager::Object::Create<UIObject>(effect_scene);
				auto img = effect_obj->AddComponent<ImageRenderer>();
				effect_obj->transform->position = { -SCREEN_W * 0.25f,0,0 };
				effect_obj->transform->scale = { SCREEN_W * 0.75f,SCREEN_H * 0.75f,1 };
				effect_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_MIDDLE;
				effect_obj->AnchorType() = UIObject::ANCHOR_TYPE::CENTER;
				auto mat = MaterialManager::GetMaterial("hunt_eff_mat");

				img->SetMaterial(mat);
				effect_duration_timer = EFFECT_DURATION;
			}
			if (!effect_text_obj) {
				effect_text_obj = SceneManager::Object::Create<UIObject>(effect_scene);
				auto text = effect_text_obj->AddComponent<Text>();
				effect_text_obj->transform->position = { SCREEN_W * 0.2f,-50,0 };
				effect_text_obj->transform->scale = { 700,170,1 };
				effect_text_obj->CanvasAnchorType() = UIObject::ANCHOR_TYPE::RIGHT_TOP;
				effect_text_obj->AnchorType() = UIObject::ANCHOR_TYPE::RIGHT_TOP;
				text->TextColor() = Color::RED;
				text->SetText(u8"逮捕完了!!");
				text->SetFontSize(80);
				text->SetAlignment(Text::ALIGNMENT::AUTO);
				text->text_speed = 4.0f;
			}
			hunt_se->PlayOneShot();
		}
	};
}



namespace NeonFade {
	//1ビルの縦横...100m×100m
	std::array<Vector3, 4> buildings = {
		Vector3(-100.0f,0.0f,290.0f),
		Vector3(-100.0f,0.0f,130.0f),
		Vector3(100.0f,0.0f,130.0f),
		Vector3(100.0f,0.0f,290.0f),
	};
	std::unique_ptr<HuntedEffectCreater> hunted_effect_creater = nullptr;

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
		ModelManager::LoadAsAnimation(u8"data/player/anim_spin.mv1", "spin");
		ModelManager::LoadAsAnimation(u8"data/player/anim_clouch_inv.mv1", "clouch_inv");
		ModelManager::LoadAsAnimation(u8"data/player/anim_spin_kick.mv1", "spin_kick");
		ModelManager::LoadAsAnimation(u8"data/player/anim_leg_sweep.mv1", "leg_sweep");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump_cool.mv1", "jump_cool");

		ModelManager::LoadAsModel(u8"data/Stage/Buildings/Ground.mv1", "stage");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/building-01_UV.mv1", "building");
		ModelManager::LoadAsModel(u8"data/Stage/megapolis/road.mv1", "high-way");


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



		{
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
		CheckForLoading();
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
			shadowmap->SetShadowMapSize(1024);
			shadowmap->SetLightDirection({ 0, -8, 5 });
			auto rec_shadowmap = SceneManager::Object::Create<ShadowMapObject>(SceneManager::GetDontDestoryOnLoadScene());
			rec_shadowmap->SetCascadeCount(2);
			rec_shadowmap->SetShadowMapSize(1024);
			rec_shadowmap->SetLightDirection({ 0, -8, 5 });
		}

		if (!light_manager) {

			light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
			light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 15,8,10 }, 0, 0, { 0,-8,5 });
			auto rec_light_manager = SceneManager::Object::Create<LightManager>(SceneManager::GetDontDestoryOnLoadScene());
			rec_light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 20,20,20 }, 0, 0, { 0,-8,5 });


			{

			}
			{

				auto mat = MaterialManager::CreateMaterial("hunt_eff_mat");
				auto tex = SceneManager::GetDontDestoryOnLoadScene()->GetCurrentCamera()->hdr;
				mat->SetTexture(tex, Material::TextureType::Diffuse);
				auto movie = TextureManager::Get("cutin_eff");
				mat->SetTexture(movie, Material::TextureType::Emission);
				mat->SetShaderPs(MaterialManager::LoadPixelShader(u8"data/shader/ps_hunted_effect.fx", u8"ps_hunted_effect"));
				mat->SetShaderVs(MaterialManager::GetDefaultMat2D()->GetVertexShader());
			}
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


			{
				auto ground = SceneManager::Object::Create<GameObject>("Ground");
				auto ground_mdl = ground->AddComponent<ModelRenderer>();
				ground_mdl->SetModel("stage");
				ground->AddComponent<RigidBody>();
				ground->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);
				ground->transform->scale = { 5,5,5 };
				ground->transform->position = { 0,0,200 };

			}
			{
				camera->transform->position = { 0,10,10 };
				camera->transform->SetAxisZ({ 0,-0.75f,-1.0f });
				camera->camera->render_type = Camera::RenderType::Deferred;
				camera->camera->camera_far = 1000.0f;
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
			auto high_way = SceneManager::Object::Create<GameObject>("HighWay");
			auto mod = high_way->AddComponent<ModelRenderer>();
			mod->SetModel("high-way");

			high_way->transform->scale = { 3.0f,3.0f,3.0f };
			high_way->AddComponent<RigidBody>();
			high_way->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);

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
		if (!hunted_effect_creater)
			hunted_effect_creater = std::make_unique<HuntedEffectCreater>();
		return Super::Init();

	}

	void SceneGame::Update()
	{
		if (!CheckForLoading())
			return;
		hunted_effect_creater->Update();
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
		//DrawBoxAA(0, 50, sample_count_timer * 50.0f, 80, Color::RED, true);
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
	{
		hunted_effect_creater.reset();
	}

	bool SceneGame::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADED)
			return true;
		loading_status = (ModelManager::GetLoadingCount() + TextureManager::GetLoadingCount() + AudioManager::GetLoadingCount()) == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADED)
			Init();
		return loading_status == LOADING_STATUS::LOADED;
	}

	void SceneGame::SubtractEnemyCount(u32 cnt)
	{
		enemy_count -= min(cnt, enemy_count);
		if (hunted_effect_creater)
			hunted_effect_creater->RegistEnemy();
	}

	void SceneGame::ClearAllEnemy()
	{
		enemy_count = 0;
		auto enems = SceneManager::Object::GetArray<Enemy>();
		for (auto& enem : enems)
			SceneManager::Object::Destroy(enem);

		if (hunted_effect_creater)
			hunted_effect_creater->ResetEffect();
	}

	bool SceneGame::IsEffectExsist() const
	{
		return hunted_effect_creater && hunted_effect_creater->IsEffectActive();
	}
	bool SceneGame::IsEffectPreparing() const
	{
		return hunted_effect_creater && hunted_effect_creater->IsEffectPreparing();
	}

}