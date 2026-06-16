#include "SceneTitle.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Managers/LightManager.h"
#include "Game/Objects/DevelopTools/LightEditor.h"
#include <functional>
#include "TitleLogo.h"
#include "OptionData.h"
#include "OptionManager.h"
#include "OptionCamera.h"
#include "OptionDeadZone.h"
#include "OptionVolume.h"
#include "OptionStart.h"
#include "OptionExit.h"
#include "SettingsManager.h"
#include "OptionSetting.h"

#include "Game/Objects/NeonFade/TitlePlayer.h"

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






	TitleLogoWP logo;
	OptionManagerWP opt_manager;
	SettingsManagerWP settings_manager;

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
	void SceneTitle::Load() {

		ModelManager::LoadAsModel(u8"data/Stage/Buildings/sky_tower5.mv1", "sky_tower5");
		ModelManager::LoadAsModel(u8"data/player/model_v2.mv1", "player_model");
		ModelManager::LoadAsAnimation(u8"data/player/anim_sitting_idle.mv1", "player_sitting");
		ModelManager::LoadAsAnimation(u8"data/player/anim_dive.mv1", "player_dive");
		ModelManager::LoadAsAnimation(u8"data/player/anim_twist_flip.mv1", "player_twist_flip");
	}
	int SceneTitle::Init() {
		{
			Vector3 title_camera_pos = { 384.22f, 1539.66f, 113.73f };
			Quaternion title_camera_rot = Quaternion(5.341f, { 0,1,0 })* Quaternion(0.224f, { 1,0,0, });
			auto cam = SceneManager::Object::Create<CameraObject>();
			cam->transform->position = title_camera_pos;
			cam->transform->rotation = title_camera_rot;
			cam->camera->SetPerspective(40);
			cam->camera->camera_far = 2000.0f;
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
			auto light_editor = SceneManager::Object::Create<LightEditor>(u8"ライトエディター");
			light_editor->file_path = "data/SceneTitle_LightData.txt";
			light_editor->Load(light_editor->file_path);
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
			auto pl = SceneManager::Object::Create<TitlePlayer>("Player");



		}

		{
			for (u32 i = 0; i < title_buildings.size(); ++i)
			{
				auto building = SceneManager::Object::Create<GameObject>("sky_tower5_" + std::to_string(i));
				building->AddComponent<ModelRenderer>()->SetModel("sky_tower5");
				building->transform->rotation = title_building_rotations[i];
				building->transform->position = title_buildings[i];
			}
		}
		return Super::Init();
	}
	void SceneTitle::Update() {
	}
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