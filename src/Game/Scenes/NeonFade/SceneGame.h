// SceneGame.h
//---------------------------------------------------------------------------
//! @file   SceneGame.h
//! @brief  メインゲームシーン
//---------------------------------------------------------------------------
#pragma once
#include "System/Scene.h"
#include "Game/Utilitys/NeonFade/StateMachines/SceneGameStateMachine.h"
#include "Game/Scenes/NeonFade/DebugCameraScene.h"
USING_PTR(CameraObject);
USING_PTR(ShadowMapObject);
USING_PTR(LightManager);
USING_PTR(LightEditor);
namespace NeonFade {
	USING_PTR(SceneGameStateMachine);
	USING_PTR(Player);
	USING_PTR(SettingsManager);

	//---------------------------------------------------------------------
	//! @class SceneGame
	//! @brief メインゲームシーンクラス
	//---------------------------------------------------------------------
	class SceneGame :
		public Scene
		//public DebugCameraScene
	{
	public:
		USING_SUPER(SceneGame);
		//! @brief リソース読み込み処理
		void Load() override;
		//! @brief 初期化処理
		int Init() override;
		//! @brief 更新処理
		void Update() override;
		//! @brief 描画前処理
		void PreDraw() override;
		//! @brief デバッグ描画（遅延）
		void LateDebugDraw() override;
		//! @brief 遅延描画処理
		void LateDraw() override;

		//! @brief デバッグ描画処理
		void DebugDraw() override;
		//! @brief 遅延描画完了イベント
		void OnLateDrawFinish() override;

		//! @brief 終了処理
		void Exit() override;
		//! @brief ローディング状態を確認する
		//! @param init 初期化フラグ(確認したときに初期化処理も行うかどうか)
		//! @return ロード完了ならtrue、まだロード中ならfalse
		bool CheckForLoading(bool init = true);

		//! @brief ゲームの一時停止状態を設定する
		//! @param pause ポーズする場合はtrue
		void PauseGame(bool pause = true);

		//! @brief ポーズメニューの有効化/無効化を設定する
		//! @param enable 有効化する場合はtrue
		void EnablePauseMenu(bool enable = true);

		//! @brief ゲームの一時停止状態を取得する
		//! @return ポーズ中であればtrue
		bool IsGamePaused() const { return is_game_paused; }
		//! @brief ゲームの一時停止可能状態を設定する
		//! @param available ポーズ可能にする場合はtrue
		void SetPauseAvailable(bool available) { is_pause_available = available; }
		//! @brief ゲームの一時停止可能状態を取得する
		//! @return ポーズ可能であればtrue
		bool IsPauseAvailable() const { return is_pause_available; }


		//! @brief 敵カウントを加算する
		//! @param cnt 加算する数
		void AddEnemyCount(u32 cnt = 1) { enemy_count += cnt; }
		//! @brief 敵カウントを減算する
		//! @param cnt 減算する数
		void SubtractEnemyCount(u32 cnt = 1);
		//! @brief 敵の総数を取得する
		//! @return 現在の敵の数
		int GetEnemyCount() const { return enemy_count; }
		//! @brief 全ての敵を消去する
		void ClearAllEnemy();
		CameraObjectWP camera; //!< メインカメラ
		PlayerWP player; //!< プレイヤーオブジェクト
		UIObjectWP text_obj; //!< テキストUIオブジェクト
		UIObjectWP hud_obj; //!< HUDオブジェクト
		std::unordered_map<std::string, UIObjectWP> ui_texts; //!< UIテキストマップ
		TextWP text_comp; //!< テキストコンポーネント
		SceneGameStateMachineUP scene_state_machine = nullptr; //!< シーンのステートマシン
		static constexpr float GAME_TIMER_MAX = 300.0f; //!< ゲームタイマー最大値（秒）
		SettingsManagerWP pause_menu_obj = nullptr; //!< ポーズメニューオブジェクト

		ShadowMapObjectWP shadowmap = nullptr; //!< シャドウマップオブジェクト
		LightManagerWP light_manager = nullptr; //!< ライトマネージャー
		//! @brief ゲームタイマーを開始する
		void StartGameTimer() { is_game_timer_started = true; }
		//! @brief ゲームタイマーをリセットする
		void ResetGameTimer() { game_timer = 0.0f; }
		//! @brief ゲームタイマーを停止する
		void StopGameTimer() { is_game_timer_started = false; }
		//! @brief ゲームタイマーの値を取得する
		//! @return 現在のタイマー時間
		float GetGameTimer() const { return game_timer; }

		/// @brief SEの音量を取得する
		/// @return 現在のSE音量
		static float GetSEVolume() { return se_volume; }
		/// @brief BGMの音量を取得する
		/// @return 現在のBGM音量
		static float GetBGMVolume() { return bgm_volume; }

		//! @brief 音量設定をロードする
		void LoadVolumeSettings();
		//! @brief カメラ設定をロードする
		void LoadCameraSettings();

		AudioPlayerWP audio_player; //!< オーディオプレイヤー
		//! シーンで使用するリソースをまとめて読み込む静的関数
		static void LoadResources();

		//! @brief チュートリアルが有効かどうか取得
		//! @return チュートリアルが有効かどうか
		bool IsTutorialEnabled() { return tutorial_enabled; }

		//! @brief チュートリアルを有効化もしくは無効化する
		//! @param enable 有効化or無効化
		void EnableTutorial(bool enable = true);
		
	private:
		float game_timer = 0.0f; //!< ゲームタイマー
		bool is_game_timer_started = false; //!< タイマー動作中フラグ
		bool is_game_paused = false; //!< ゲーム一時停止フラグ
		bool is_pause_available = false; //!< ゲーム一時停止可能フラグ
		u32 enemy_count = 0; //!< 現在の敵の数
		static inline float bgm_volume = 1.0f; //!< BGM音量
		static inline float se_volume = 1.0f; //!< SE音量
		LightEditorWP light_editor = nullptr; //!< ライトエディタオブジェクト


		static inline bool tutorial_enabled = true;

	};
}