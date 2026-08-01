//---------------------------------------------------------------------------
//! @file   SceneGameState_Show.cpp
//! @brief  SceneGameState_Showの実装。ゲームの演出シーン状態処理を行う
//---------------------------------------------------------------------------
#include "SceneGameState_Show.h"
#include "Game/Scenes/NeonFade/SceneGame.h"

#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"

#include "Game/Objects/NeonFade/TutorialUI.h"

namespace NeonFade
{

	//! @brief チュートリアル1コマ分のパラメータ
	struct TutorialParam {
		std::string movie_name = "";
		std::string text_message = "";
		PadButton button = PadButton::Back;
	};

	//! @brief チュートリアルUIを管理するクラス
	class Tutorial : public GameObject
	{
	public:
		USING_SUPER(Tutorial);
		Tutorial(std::function<void()>&& on_init_, std::function<void()>&& on_exit_, const TutorialParam& param)
			:GameObject()
		{
			// チュートリアルUIの初期化と終了時のコールバック関数を設定する
			on_init = std::move(on_init_);
			on_exit = std::move(on_exit_);

			// チュートリアルUIの表示に必要な情報を保持する
			tutorial_param = param;
		}
		int Init() override {
			//もしon_initが設定されていれば、初期化時に呼び出す
			if (on_init) {
				on_init();
				on_init = nullptr;	//二重呼び出しを防ぐために、on_initをnullptrにする
			}

			//自身が管理するTutorialUIオブジェクトを作成する
			//可読性が低いので、エイリアスを作る
			auto& param = tutorial_param;
			ui_obj = SceneManager::Object::Create<TutorialUI>
				("tutorial_ui", param.movie_name, param.text_message, param.button);

			// TutorialUIオブジェクトを表示する
			ui_obj->WakeUp();

			return Super::Init();

		}
		void Update() override {
			// TutorialUIオブジェクトが存在しない(閉じられた)場合は、自身を破棄する
			if (!ui_obj) {
				SceneManager::Object::Destroy(shared_from_this());

				//Exitが呼ばれるのは次のフレームになるので、ここでon_exitを呼び出す

				//もしon_exitが設定されていれば、呼び出す
				if (on_exit) {
					on_exit();
					on_exit = nullptr;	//二重呼び出しを防ぐために、on_exitをnullptrにする
				}

			}

		}
		void Exit() override {
			// 何かの手違いでTutorialUIオブジェクトが存在する場合は、破棄する
			if (ui_obj) {
				SceneManager::Object::Destroy(ui_obj.lock());
				ui_obj.reset();
			}
			if (on_exit) {
				on_exit();
				on_exit = nullptr;	//二重呼び出しを防ぐために、on_exitをnullptrにする
			}
		}

	private:
		std::function<void()> on_init;		//<! チュートリアルUIの初期化時に呼び出されるコールバック関数
		std::function<void()> on_exit;		//<! チュートリアルUIの終了時に呼び出されるコールバック関数
		UIObjectWP ui_obj;				//<! TutorialUIオブジェクトへの弱参照
		TutorialParam tutorial_param;		//<! チュートリアルUIの表示に必要な情報を保持する構造体
	};




	//! @brief 複数個のチュートリアルを順番に再生するヘルパー関数
	void CreateTutorialSequence(
		const std::vector<TutorialParam>& params,
		std::function<void()> on_start = nullptr,
		std::function<void()> on_complete = nullptr,
		std::function<bool()> check_next = nullptr,
		size_t index = 0)
	{
		if (params.empty() || index >= params.size()) return;

		bool is_first = (index == 0);
		bool is_last = (index == params.size() - 1);

		// 最初の1コマ目でのみ全体の開始処理（ポーズなど）を行う
		auto on_init = [is_first, on_start]() {
			if (is_first && on_start) {
				on_start();
			}
			};

		// 閉じた時の処理：最後なら終了処理、途中なら次のチュートリアルを生成
		auto on_exit = [params, on_complete, check_next, index, is_last]() {
			if (is_last) {
				if (on_complete) {
					on_complete();
				}
			}
			else {
				// 次のチュートリアルを再帰的に生成
				//もしcheck_nextが設定されていなければ、無条件で次のチュートリアルを生成する
				if (!check_next) {
					CreateTutorialSequence(params, nullptr, on_complete, check_next, index + 1);
					return;
				}
				// check_nextが設定されていれば、条件を満たす場合のみ次のチュートリアルを生成する
				if (check_next && check_next()) {
					CreateTutorialSequence(params, nullptr, on_complete, check_next, index + 1);
					return;
				}
				// 条件を満たさない場合は、次のチュートリアルを生成せずに終了処理を呼び出す
				if (on_complete)
					on_complete();
			}
			};

		const auto& p = params[index];

		SceneManager::Object::Create<Tutorial>(
			"tutorial",
			std::move(on_init),
			std::move(on_exit),
			p
		);
	}

	std::vector<std::vector<TutorialParam>> sequence = {
		{

			{ "tutorial_1", u8"移動、ダッシュ：\n左スティックで移動ができます。\n移動中にスティックを押し込んでダッシュと歩きを切り替えられます", PadButton::LeftStick },
			{ "tutorial_2", u8"攻撃1:\nAボタン連打で攻撃できます。\n3段目まで存在し、スティック入力で攻撃方向を変更できます", PadButton::Button1 },
			{ "tutorial_3", u8"攻撃2:\nXボタンを押すと足払い攻撃で付近の敵を\n上方にノックバックさせます。", PadButton::Button3 },
			{ "tutorial_4", u8"攻撃3:\nYボタンを押すと前方に移動しながらキックを行います。\nノックバックとダメージが少し大きめです", PadButton::Button4 },
			{ "tutorial_5", u8"時間制限:\n5分間で全ての敵を倒してください。\n初めに出てくる敵を全員倒すと、一度だけ再出現します。\nそれらを倒すとクリアになります", PadButton::Back },
		},
		{
			{ "tutorial_6", u8"敵の種類:\n敵は団体行動を行うタイプと、単体で行動するタイプがいます。", PadButton::Back  },
			{ "tutorial_7", u8"敵の挙動:\n団体行動する敵は、リーダーの指示でプレイヤーに攻撃してきます。", PadButton::Back  },
			{ "tutorial_8", u8"敵の挙動:\n単体で行動する敵は、近くに弱った敵がいると庇いに行きます。", PadButton::Back  },
			{ "tutorial_9", u8"効率のいい倒し方:\n攻撃、あるいは庇いに集まってきた敵には隙があります。\nまとまったところを一気に叩きましょう。", PadButton::Back  },
			{ "tutorial_10", u8"敵ノックバック時の挙動:\nノックバックした敵は、近くにいる味方の敵を巻き込んで\n連鎖的にノックバックします\nまとめてノックバックさせると有利です。", PadButton::Back  },
		},
		{
			{ "tutorial_11", u8"回避:\n敵の攻撃を回避するには、左トリガーを使用します。\n敵が一気に攻撃してきた場合は回避しましょう", PadButton::LTrigger  },
			{ "tutorial_12", u8"空中攻撃:\n空中で攻撃を行うと、空中攻撃になります。\n空中攻撃はダメージが非常に高く、攻撃範囲も広いです。\nたくさん集まってきた敵を巻き込みましょう", PadButton::Button1 },
			{ "tutorial_13", u8"ポーズ機能:\nスタートボタンでポーズ画面を開けます。\nポーズ画面では、カメラや音量、コントローラーのデッドゾーン\nなどの設定ができます", PadButton::Start }
		}
	};

	void SceneGameState_Show::CreateTutorial(u32 index) {



		// チュートリアル全体の開始時（ポーズ適用）
		auto on_start = [this]() {
			//もしプレイヤーがポーズ画面を開いていたら、強制的に閉じる
			owner_scene_game->EnablePauseMenu(false);

			//プレイヤーが手動でポーズor解除できないようにする
			owner_scene_game->SetPauseAvailable(false);
			//強制的にポーズ状態にする
			owner_scene_game->PauseGame(true);
			};

		// チュートリアル全体の終了時（ポーズ解除）
		auto on_complete = [this]() {
			//プレイヤーが手動でポーズor解除できるようにする
			owner_scene_game->SetPauseAvailable(true);
			//強制的にポーズ状態を解除する
			owner_scene_game->PauseGame(false);
			};

		auto check_next = [this]() {
			//チュートリアルが有効化されているなら、次も表示する
			return owner_scene_game->IsTutorialEnabled();
			};


		CreateTutorialSequence(sequence[index], std::move(on_start), std::move(on_complete), std::move(check_next));

	}



	SceneGameState_Show::SceneGameState_Show(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;

		timer_text = owner_scene_game->ui_texts["txt_time"];
	}
	void SceneGameState_Show::OnEnter(ISceneStateMachine* machine)
	{
		EnemyFactory factory;
		factory.SetSpawnPosition({ 0,5,200 });
		factory.SetSpawnRadius(50);
		u32 teams = FileSystem::IniFileManager::GetInt("Game", "team_counts_1", 1, "data/config.ini");
		u32	enemys_per_team = FileSystem::IniFileManager::GetInt("Game", "members_per_team_1", 5, "data/config.ini");
		bool is_team_mode = FileSystem::IniFileManager::GetBool("Game", "teamed_enemy_1", false, "data/config.ini");
		{
			if (is_team_mode) {
				for (u32 i = 0; i < teams; ++i) {

					auto enemy_team = factory.MakeEnemyTeam(enemys_per_team, owner_scene_game->player);
					enemy_teams.push_back(std::move(enemy_team));
				}
			}
			else {

				for (u32 i = 0; i < teams * enemys_per_team; ++i)
				{
					factory.MakeBasicEnemy(owner_scene_game->player);
				}
			}

			Time::ResetTime();
		}
		owner_scene_game->AddEnemyCount(teams * enemys_per_team);
		owner_scene_game->text_comp->SetText(u8"Show State");
		owner_scene_game->ResetGameTimer();
		owner_scene_game->StartGameTimer();
		owner_scene_game->SetPauseAvailable(true);
		timer_text->WakeUp();





		//チュートリアルで説明すべき点:
		//1.移動+ダッシュについて
		//2.攻撃1について
		//3.攻撃2について
		//4.攻撃3について
		//5.時間制限について

		//6.敵の種類(リーダーとメンバーがいるよ)
		//7.敵の挙動(集まってくるから大技を使うといいよ)
		//8.敵ノックバック時の挙動(連鎖するよ)
		//9.回避について

		//10.空中攻撃について
		//11.ポーズ機能について



		// 順次再生の実行
		if (owner_scene_game->IsTutorialEnabled())
			CreateTutorial(0);

	}
	void SceneGameState_Show::OnExit(ISceneStateMachine* machine)
	{
		owner_scene_game->EnableTutorial(false);
		enemy_teams.clear();
	}
	void SceneGameState_Show::Update(ISceneStateMachine* machine, float dt)
	{
		if (Input::GetPadButtonDown(0, PadButton::Back)) {
			owner_scene_game->EnableTutorial(false);
			if (auto tutorial = SceneManager::Object::Get<Tutorial>())
				SceneManager::Object::Destroy(tutorial);
		}
		//ポーズ中はチュートリアルのタイマーを進めない
		//チュートリアルが無効化されていてもタイマーを進めない
		if (owner_scene_game->IsGamePaused() || !owner_scene_game->IsTutorialEnabled())
			return;

		//タイマーを進めて、タイミングが来たらチュートリアルを表示する
		float tutorial_timing_counter_prev = tutorial_timing_counter;
		tutorial_timing_counter += Time::DeltaTime();

		if (tutorial_timing_counter > TUTORIAL_TIMING_ABOUT_ENEMY) {
			if (tutorial_timing_counter_prev <= TUTORIAL_TIMING_ABOUT_ENEMY)
				CreateTutorial(1);
		}
		if (tutorial_timing_counter > TUTORIAL_TIMING_ABOUT_EXTRA) {
			if (tutorial_timing_counter_prev <= TUTORIAL_TIMING_ABOUT_EXTRA)
				CreateTutorial(2);
		}
	}



}