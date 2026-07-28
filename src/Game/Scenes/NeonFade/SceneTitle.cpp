// SceneTitle.cpp
#include "SceneTitle.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Managers/LightManager.h"
#include "Game/Objects/DevelopTools/LightEditor.h"
#include <functional>
#include "Game/Objects/NeonFade/TitleObjects/TitleLogo.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionData.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionManager.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionCamera.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionDeadZone.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionVolume.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionStart.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionExit.h"
#include "Game/Objects/NeonFade/TitleObjects/SettingsManager.h"
#include "Game/Objects/NeonFade/TitleObjects/OptionSetting.h"

#include "Game/Objects/NeonFade/TitlePlayer.h"
#include "Game/Utilitys/NeonFade/StateMachines/TitlePlayerStateMachine.h"

namespace NeonFade {

	//必要なのは、
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







	std::array<Vector3, 4> title_buildings = {
	Vector3(-400.193f,0.0f,191.397f),
	Vector3(-400.193f,0.0f,695.987f),
	Vector3(243.596f,0.0f,521.991f),
	Vector3(400.193f,0.0f,104.398f),
	};
	std::array<Quaternion, 4> title_building_rotations = {
		Quaternion(1.571f,{0,1,0}),
		Quaternion(0.0f,{0,1,0}),
		Quaternion(0.0f,{0,1,0}),
		Quaternion(4.712f,{0,1,0}),
	};
	//メインシーンで使用するリソースを、タイトルシーンのうちに読み込み始めると切り替えがスムーズになる
	//先に、タイトルシーンで必要なリソースを読み込む
	/// @brief タイトルシーンで使用するリソースをロードする
	/// @details メインシーン用のモデルやアニメーションを先行して読み込む
	void SceneTitle::Load() {
		// 背景ビルとプレイヤーのモデルを読み込む
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower5.mv1", "sky_tower5");
		ModelManager::LoadAsModel(u8"data/player/model_v2.mv1", "player_model");
		// プレイヤーの各種アニメーションを読み込む
		ModelManager::LoadAsAnimation(u8"data/player/anim_sitting_idle.mv1", "player_sitting");
		ModelManager::LoadAsAnimation(u8"data/player/anim_dive.mv1", "player_dive");
		ModelManager::LoadAsAnimation(u8"data/player/anim_twist_flip.mv1", "player_twist_flip");
		TextureManager::Load(u8"data/Textures/TitleLogo.png", "title_logo_texture");
		static u64 capture_time = 0.0f;
		while (capture_time < 5) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			capture_time++;
		}
	}
	/// @brief タイトルシーンの初期化を行う
	/// @return 初期化の成否を示す整数値
	int SceneTitle::Init() {
		{
			// 既存のゲームシーンがあれば破棄する
			auto game = SceneManager::GetScene<SceneGame>();
			if (game)
				SceneManager::Destroy(game);
		}

		{
			// カメラの初期位置と回転を設定する
			Vector3 title_camera_pos = { 384.22f, 1539.66f, 113.73f };
			Quaternion title_camera_rot = Quaternion(5.341f, { 0,1,0 }) * Quaternion(0.224f, { 1,0,0, });
			// カメラオブジェクトを作成しパラメータを割り当てる
			auto cam = SceneManager::Object::Create<CameraObject>();
			cam->transform->position = title_camera_pos;
			cam->transform->rotation = title_camera_rot;
			cam->camera->SetPerspective(40);
			cam->camera->camera_far = 2000.0f;
			cam->camera->render_type = Camera::RenderType::Deferred;
		}
		{
			// シャドウマップを作成して設定を適用する
			auto shadowmap = SceneManager::Object::Create<ShadowMapObject>("ShadowMap");
			shadowmap->SetCascadeCount(4);
			shadowmap->SetShadowMapSize(1024);
			shadowmap->SetLightDirection({ 0, -8, 5 });
		}
		{
			// ライト関連のマネージャーとエディタを作成する
			auto light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
			auto light_editor = SceneManager::Object::Create<LightEditor>(u8"ライトエディター");
			light_editor->file_path = "data/SceneTitle_LightData.txt";
			// ライト設定ファイルを読み込む
			light_editor->Load(light_editor->file_path);
		}

		// ロゴオブジェクトが存在しなければ作成する
		if (!logo)
			logo = SceneManager::Object::Create<TitleLogo>();

		// オプションマネージャーと検証用オプションの作成
		if (!opt_manager && !settings_manager) {
			// オプション管理用オブジェクトを作成する
			opt_manager = SceneManager::Object::Create<OptionManager>("OptionManager");
			// 設定管理用オブジェクトを作成する
			settings_manager = SceneManager::Object::Create<SettingsManager>("SettingsManager");
			settings_manager->auto_close = true;
			settings_manager->close_key = KeyCode::Minus;
			settings_manager->close_pad_button = PadButton::Button1;


			// 設定マネージャーにオプションマネージャーを紐づける
			if (auto sm = settings_manager.lock()) {
				sm->main_opts = opt_manager;
			}

			// スタート用オプション項目を作成して配置する
			auto opt_start = SceneManager::Object::Create<OptionStart>("OptionStart");
			opt_start->transform->position = { 450, -350, 1 };

			// 設定用オプション項目を作成して配置する
			auto opt_setting = SceneManager::Object::Create<OptionSetting>("OptionSetting");
			opt_setting->settings_manager = settings_manager;
			opt_setting->transform->position = { 450, -420, 1 };

			// 終了用オプション項目を作成して配置する
			auto opt_exit = SceneManager::Object::Create<OptionExit>("OptionExit");
			opt_exit->transform->position = { 450, -490, 1 };

			// 各オプション項目をマネージャーに登録する
			opt_manager->AddOption(SafeStaticCast<OptionData>(opt_start));
			opt_manager->AddOption(SafeStaticCast<OptionData>(opt_setting));
			opt_manager->AddOption(SafeStaticCast<OptionData>(opt_exit));
		}

		{
			// タイトル用のプレイヤーオブジェクトを作成する
			auto pl = SceneManager::Object::Create<TitlePlayer>("Player");



		}

		{
			// 定義された位置に背景用のビルモデルを配置する
			for (u32 i = 0; i < title_buildings.size(); ++i)
			{
				auto building = SceneManager::Object::Create<GameObject>("sky_tower5_" + std::to_string(i));
				building->AddComponent<ModelRenderer>()->SetModel("sky_tower5");
				building->transform->rotation = title_building_rotations[i];
				building->transform->position = title_buildings[i];
			}
		}
		// 親クラスの初期化関数を呼び出す
		return Super::Init();
	}
}