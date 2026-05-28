#include "SceneTitle.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Managers/LightManager.h"
#include <functional>

namespace NeonFade {

	//タイトルシーンの構成
	//イメージ図:
	//+------------------------------------------------------------------------------------------------+
	//|			+-----------------------+															   |
	//|			|	   タイトルロゴ	    |															   |
	//|			+-----------------------+															   |
	//|								(キー)を押して設定画面へ	<選択中のものを点滅させ、					   |
	//|								(キー)を押してスタート		<少し大きくするなどのアニメーションをつける	   |
	//|																_________________________		   |
	//|			_________________________						  ／							／		   |
	//|		  ／							／						／________________________／  |		   |
	//|		／________________________／  |				    	|						 |	 |		   |
	//|		|						 |	 |						|						 |	 |		   |
	//|		|						 |	 |						|						 |	 |		   |
	//|		|						 |	 |						|		 ビル			 |	 |		   |
	//|		|		 ビル			 |	 |						|						 |	 |		   |
	//|		|						 |	 |						|						 |	 |		   |
	//|		|						 |	 |						|	 __________ 		 |	 |		   |
	//|		|						 |	 |						|	|プレイヤー |			 |	 |		   |
	//|		|						 |	 |						|	| キャラ    |		 |   |		   |
	//|		|	_____________________|___|_						| __|          |_________|___|________ |
	//|		| ／			ビル				／ |					    ／  |	       |					   |
	//|		／________________________／   |					  ／		|	       |<-いい感じのアニメーション |
	//|		|						 |	  |					／	    |	       |					   |
	//+------------------------------------------------------------------------------------------------+

	//という訳で必要なのは、
	//ロゴ(テクスチャまたはフォント)
	//選択肢(フォント)
	//ビル(モデル)
	//プレイヤーキャラ(モデル、アニメーション)
	//地面(モデル)->写す場合
	//あとはBGMとSEも必要かもしれない<-ここら辺はプロトタイプができてからでもよき

	//前やって没にした、後ろでパトカーを走らせるとかも面白そうではあるが、ゲーム中には出てこない要素なので、
	//プロトタイプ完成後に様子を見て決める

	//んで、必要なクラスは、
	//SceneTitle(このシーン)
	//TitleLogo(ロゴ)->テクスチャ(またはテキストコンポーネント)だけ持ってればおｋ
	// 
	//OptionManager(選択肢管理クラス)->選択肢クラスのリストを持つ
	//Option(選択肢の実体)->テキストコンポーネントと、選択時・非選択時・待機・決定時の処理を持つ
	//上2つは、タイトル画面だけでなくメニュー画面の設定項目管理にも流用できるとよいかも
	// 
	//TitlePlayer(プレイヤーキャラ)->モデルとアニメーションを持ち、待機・選択・決定のアニメーションを切り替える
	//BackGroundModels(背景モデル)->モデルを置くだけでいいので、特に特殊なことはしない。

	//ファイル分けは後にして、とりあえず必要なクラスを実装していく
	USING_PTR(TitleLogo);
	class TitleLogo :public UIObject {

	public:
		USING_SUPER(TitleLogo);
		int Init() override {
			auto txt = AddComponent<Text>();
			txt->SetFont(u8"meirio");
			txt->SetFontSize(190);
			txt->TextColor() = Color::DARK_RED;
			txt->SetText(u8"NEON FADE");
			logo_text = txt;
			anchor_type = LEFT_TOP;
			canvas_anchor_type = LEFT_TOP;
			transform->scale = Vector3(700, 200, 1);
			transform->position = { 120,-20,1 };
			return Super::Init();
		}
	private:
		TextWP logo_text;
	};


	USING_PTR(OptionData);
	class OptionData : public UIObject {
	public:
		USING_SUPER(OptionData);
		std::function<void()> on_select;
		std::function<void()> on_deselect;
		std::function<void()> on_decide;
		std::function<void()> on_left;
		std::function<void()> on_right;
		std::function<void()> on_idle;

		std::vector<OptionDataWP> child_options;
		bool is_selected = false;

		int Init() override {
			auto txt = AddComponent<Text>();
			txt->SetFont(u8"meirio");
			txt->SetFontSize(48);
			txt->TextColor() = Color::WHITE;
			txt->SetText(u8"OPTION");
			text_comp = txt;
			anchor_type = LEFT_TOP;
			canvas_anchor_type = LEFT_TOP;
			transform->scale = Vector3(200, 60, 1);

			base_scale = transform->scale;
			return Super::Init();
		}

		void Update() override {
			Super::Update();

			// アニメーション (選択時はスケールを少し大きくする)
			Vector3 target_scale = is_selected ? base_scale * 1.2f : base_scale;
			transform->scale = transform->scale + (target_scale - transform->scale) * 0.1f;

			if (is_selected) {
				if (on_select) on_select();
			}
			else {
				if (on_idle) on_idle();
			}
		}

		void Select() {
			is_selected = true;
			text_comp.lock()->TextColor() = Color::YELLOW;
		}

		void Deselect() {
			is_selected = false;
			text_comp.lock()->TextColor() = Color::WHITE;
			if (on_deselect) on_deselect();
		}

		void Decide() {
			if (on_decide) on_decide();
		}

		void Left() {
			if (on_left) on_left();
		}

		void Right() {
			if (on_right) on_right();
		}

		void SetText(const std::string& str) {
			if (auto t = text_comp.lock()) {
				t->SetText(str);
			}
		}

	private:
		TextWP text_comp;
		Vector3 base_scale;
	};

	USING_PTR(OptionManager);
	class OptionManager : public UIObject {
	public:
		USING_SUPER(OptionManager);

		int Init() override {
			return Super::Init();
		}

		void AddOption(OptionDataWP option) {
			options.push_back(option);
			if (options.size() == 1) { // 最初の追加時に選択状態にする
				cursor_index = 0;
				if (auto o = options[0].lock()) o->Select();
			}
		}

		const std::vector<OptionDataWP>& GetOptions() const {
			return options;
		}

		void Update() override {
			if (options.empty()) return;

			if (Input::GetKeyDown(KeyCode::Up) || Input::GetPadButtonDown(0, PadButton::Up)) {
				MoveCursor(-1);
			}
			if (Input::GetKeyDown(KeyCode::Down) || Input::GetPadButtonDown(0, PadButton::Down)) {
				MoveCursor(1);
			}
			if (Input::GetKeyDown(KeyCode::Left) || Input::GetPadButtonDown(0, PadButton::Left)) {
				if (auto o = options[cursor_index].lock()) {
					o->Left();
				}
			}
			if (Input::GetKeyDown(KeyCode::Right) || Input::GetPadButtonDown(0, PadButton::Right)) {
				if (auto o = options[cursor_index].lock()) {
					o->Right();
				}
			}
			if (Input::GetKeyDown(KeyCode::Return) || Input::GetKeyDown(KeyCode::Space) || Input::GetPadButtonDown(0, PadButton::Button2)) {
				if (auto o = options[cursor_index].lock()) {
					o->Decide();
				}
			}
		}
		void OnSleep() override {
			for (auto& opt : options) {
				if (auto o = opt.lock()) {
					o->Sleep();
				}
			}
		}
		void OnWakeUp() override {
			for (auto& opt : options) {
				if (auto o = opt.lock()) {
					o->WakeUp();
				}
			}
		}

	private:
		std::vector<OptionDataWP> options;
		int cursor_index = 0;

		void MoveCursor(int dir) {
			if (auto current = options[cursor_index].lock()) {
				current->Deselect();
			}

			cursor_index += dir;
			if (cursor_index < 0) cursor_index = (int)options.size() - 1;
			if (cursor_index >= (int)options.size()) cursor_index = 0;

			if (auto next = options[cursor_index].lock()) {
				next->Select();
			}
		}
	};


	USING_PTR(OptionCamera);
	class OptionCamera : public OptionData {
	public:
		USING_SUPER(OptionCamera);
		u8 cam_mode = 0;
		enum CameraMode {
			NORMAL = 0,
			INVERT_X = 1,
			INVERT_Y = 2,
			INVERT_XY = 3,
			CAMMODE_MAX = 4
		};
		static std::string CameraModeToString(CameraMode mode) {
			switch (mode) {
			case NORMAL: return u8"カメラ操作：通常";
			case INVERT_X: return u8"カメラ操作：X軸反転";
			case INVERT_Y: return u8"カメラ操作：Y軸反転";
			case INVERT_XY: return u8"カメラ操作：X/Y軸反転";
			default: return u8"カメラ操作：UNKNOWN";
			}
		}
		static void ApplyCameraOption(CameraMode mode) {
			FileSystem::IniFileManager::SetBool("Camera", "InvertX", (mode == INVERT_X || mode == INVERT_XY), u8"data/config.ini");
			FileSystem::IniFileManager::SetBool("Camera", "InvertY", (mode == INVERT_Y || mode == INVERT_XY), u8"data/config.ini");
		}
		int Init() override {
			int ret = Super::Init();
			anchor_type = CENTER;
			canvas_anchor_type = CENTER;
			SetText(u8"カメラ操作：通常");
			on_right = [this]() {
				cam_mode++;
				cam_mode %= CAMMODE_MAX;
				SetText(CameraModeToString(static_cast<CameraMode>(cam_mode)));
				ApplyCameraOption(static_cast<CameraMode>(cam_mode));
				};
			on_left = [this]() {
				cam_mode += CAMMODE_MAX - 1; // -1 mod CAMMODE_MAX
				cam_mode %= CAMMODE_MAX;
				SetText(CameraModeToString(static_cast<CameraMode>(cam_mode)));
				ApplyCameraOption(static_cast<CameraMode>(cam_mode));
				};
			return ret;
		}
	};

	USING_PTR(OptionVolume);
	class OptionVolume : public OptionData {
	public:
		USING_SUPER(OptionVolume);
		int vol = 100;
		int Init() override {
			int ret = Super::Init();
			anchor_type = CENTER;
			canvas_anchor_type = CENTER;
			SetText(u8"VOLUME: 100%");
			on_left = [this]() {
				vol -= 10;
				if (vol < 0) vol = 0;
				SetText(u8"VOLUME: " + std::to_string(vol) + "%");
				};
			on_right = [this]() {
				vol += 10;
				if (vol > 100) vol = 100;
				SetText(u8"VOLUME: " + std::to_string(vol) + "%");
				};
			return ret;
		}
	};

	USING_PTR(OptionStart);
	class OptionStart : public OptionData {
	public:
		USING_SUPER(OptionStart);
		int Init() override {
			int ret = Super::Init();
			SetText(u8"GAME START");
			on_decide = []() { SceneManager::Load<SceneGame>(); };
			return ret;
		}
	};

	USING_PTR(OptionExit);
	class OptionExit : public OptionData {
	public:
		USING_SUPER(OptionExit);
		int Init() override {
			int ret = Super::Init();
			SetText(u8"EXIT");
			on_decide = []() { SceneManager::CloseApplication(); };
			return ret;
		}
	};

	// Settings Manager
	USING_PTR(SettingsManager);
	class SettingsManager : public UIObject {
	public:
		USING_SUPER(SettingsManager);
		enum class State {
			Hidden,
			FadingIn,
			Visible,
			FadingOut
		};

		State current_state = State::Hidden;
		float fade_timer = 0.0f;
		const float fade_duration = 0.3f;

		OptionManagerWP settings_opts;
		OptionManagerWP main_opts;

		int Init() override {
			settings_opts = SceneManager::Object::Create<OptionManager>("SettingsOptionManager");
			settings_opts->UseBackGround() = true;
			settings_opts->BackGroundColor() = Color(0, 0, 0, 0.7f);
			auto sm = settings_opts.lock();
			if (sm) {
				sm->transform->position = { 0, 0, 0 };
				sm->transform->scale = { 0.0f, 1.0f, 1.0f };
			}

			auto opt_cam = SceneManager::Object::Create<OptionCamera>("OptionCamera");
			opt_cam->transform->position = { 0, 0, 1 };

			auto opt_vol = SceneManager::Object::Create<OptionVolume>("OptionVolume");
			opt_vol->transform->position = { 0, -70, 1 };

			if (sm) {
				sm->AddOption(SafeStaticCast<OptionData>(opt_cam));
				sm->AddOption(SafeStaticCast<OptionData>(opt_vol));
				sm->Sleep();
			}
			return Super::Init();
		}

		void Update() override {
			auto sm = settings_opts.lock();
			if (!sm) return;

			Vector3 scale_closed = { 0.0f, 1.0f, 1.0f };
			Vector3 scale_open = { 600.0f, 400.0f, 1.0f };

			if (current_state == State::FadingIn) {
				fade_timer += Time::DeltaTime();
				float t = std::clamp(fade_timer / fade_duration, 0.0f, 1.0f);

				float easeT = 1.0f - std::pow(1.0f - t, 3.0f);
				sm->transform->scale = Lerp(scale_closed, scale_open, easeT);

				if (t >= 1.0f) {
					current_state = State::Visible;
					sm->WakeUp();
				}
			}
			else if (current_state == State::FadingOut) {
				fade_timer += Time::DeltaTime();
				float t = std::clamp(fade_timer / fade_duration, 0.0f, 1.0f);

				float easeT = t * t * t;
				sm->transform->scale = Lerp(scale_open, scale_closed, easeT);

				if (t >= 1.0f) {
					current_state = State::Hidden;
				}
			}
			else if (current_state == State::Visible) {
				if (Input::GetKeyDown(KeyCode::Escape) || Input::GetPadButtonDown(0, PadButton::Button1)) {

					Close();
				}
			}
		}

		void Open() {
			current_state = State::FadingIn;
			fade_timer = 0.0f;
			if (auto sm = settings_opts.lock()) {
				sm->transform->scale = { 0.0f, 1.0f, 1.0f };
				sm->WakeUp(); // keep input disabled during animation
			}
			// Disable main options
			if (auto mm = main_opts.lock()) {
				mm->Sleep();
			}
		}

		void Close() {
			current_state = State::FadingOut;
			fade_timer = 0.0f;
			if (auto sm = settings_opts.lock()) {
				sm->Sleep(); // disable input immediately
			}
			// Re-enable main options
			if (auto mm = main_opts.lock()) {
				mm->WakeUp();
			}
		}

		void Exit() override {
			if (auto o = settings_opts.lock()) {
				SceneManager::Object::Destroy(o);
			}
			Super::Exit();
		}
	};

	USING_PTR(OptionSetting);
	class OptionSetting : public OptionData {
	public:
		USING_SUPER(OptionSetting);
		SettingsManagerWP settings_manager;
		int Init() override {
			int ret = Super::Init();
			SetText(u8"SETTINGS");
			on_decide = [this]() {
				if (auto sm = settings_manager.lock()) {
					sm->Open();
				}
				};
			return ret;
		}
	};


	TitleLogoWP logo;
	OptionManagerWP opt_manager;
	SettingsManagerWP settings_manager;

	std::array<Vector3, 4> title_buildings = {
	Vector3(-100.0f,0.0f,290.0f),
	Vector3(-100.0f,0.0f,130.0f),
	Vector3(100.0f,0.0f,130.0f),
	Vector3(100.0f,0.0f,290.0f),
	};
	std::array<float, 4> title_building_rotations = {
		0.0f,
		90.0f,
		-90.0f,
		180.0f
	};


	//メインシーンで使用するリソースを、タイトルシーンのうちに読み込み始めると切り替えがスムーズになる
	//先に、タイトルシーンで必要なリソースを読み込む
	void SceneTitle::Load() {

		ModelManager::LoadAsModel(u8"data/Stage/Buildings/building-01_UV.mv1", "building");
		ModelManager::LoadAsModel(u8"data/player/model.mv1", "player_model");
		ModelManager::LoadAsAnimation(u8"data/player/anim_sitting_idle.mv1", "player_sitting");
	}
	int SceneTitle::Init() {
		{
			auto cam = SceneManager::Object::Create<CameraObject>();
			cam->transform->position = { 54,182,52 };
			cam->transform->SetRotation({ 10,-20,0 });
			cam->camera->SetPerspective(40);
			cam->camera->camera_far = 1000.0f;
			cam->camera->render_type = Camera::RenderType::Deferred;
		}
		if (true) {
			auto shadowmap = SceneManager::Object::Create<ShadowMapObject>("ShadowMap");
			shadowmap->SetCascadeCount(4);
			shadowmap->SetShadowMapSize(1024);
			shadowmap->SetLightDirection({ 0, -8, 5 });
		}
		{
			auto light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
			light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 20,20,20 }, 0, 0, { 0,-8,5 });
		}
		logo = SceneManager::Object::Create<TitleLogo>();

		// オプションマネージャーと検証用オプションの作成
		opt_manager = SceneManager::Object::Create<OptionManager>("OptionManager");
		settings_manager = SceneManager::Object::Create<SettingsManager>("SettingsManager");
		if (auto sm = settings_manager.lock()) {
			sm->main_opts = opt_manager;
		}

		auto opt_start = SceneManager::Object::Create<OptionStart>("OptionStart");
		opt_start->transform->position = { 450, -250, 1 };

		auto opt_setting = SceneManager::Object::Create<OptionSetting>("OptionSetting");
		opt_setting->settings_manager = settings_manager;
		opt_setting->transform->position = { 450, -320, 1 };

		auto opt_exit = SceneManager::Object::Create<OptionExit>("OptionExit");
		opt_exit->transform->position = { 450, -390, 1 };

		opt_manager->AddOption(SafeStaticCast<OptionData>(opt_start));
		opt_manager->AddOption(SafeStaticCast<OptionData>(opt_setting));
		opt_manager->AddOption(SafeStaticCast<OptionData>(opt_exit));

		{
			auto pl = SceneManager::Object::Create<GameObject>("Player");
			pl->AddComponent<ModelRenderer>()->SetModel("player_model");
			auto pl_anim = pl->AddComponent<Animator>();
			pl_anim->SetAnimation("player_sitting");
			pl_anim->Play("player_sitting", true);
			pl->transform->position = { 53,172,82 };
			pl->transform->SetRotation({ 0,60,0 });
			pl->transform->scale = { 0.05f,0.05f,0.05f };


		}

		{
			for (u32 i = 0; i < title_buildings.size(); ++i)
			{
				auto building = SceneManager::Object::Create<GameObject>("Building" + std::to_string(i));
				building->AddComponent<ModelRenderer>()->SetModel("building");
				building->transform->scale = { 2.0f,2.0f,2.0f };
				building->transform->SetRotation({ 0,title_building_rotations[i],0 });
				building->transform->position = title_buildings[i];
			}
		}
		return Super::Init();
	}
	void SceneTitle::Update() {}
	void SceneTitle::Exit() {
		if (logo)
			SceneManager::Object::Destroy(logo.lock());

		if (opt_manager) {
			auto opt_mgr = opt_manager.lock();
			for (auto& opt : opt_mgr->GetOptions()) {
				if (auto o = opt.lock()) {
					SceneManager::Object::Destroy(o);
				}
			}
			SceneManager::Object::Destroy(opt_mgr);
		}

		if (settings_manager) {
			SceneManager::Object::Destroy(settings_manager.lock());
		}

		Super::Exit();
	}
}