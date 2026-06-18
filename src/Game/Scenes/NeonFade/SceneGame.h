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
namespace NeonFade {
	USING_PTR(SceneGameStateMachine);
	USING_PTR(Player);
	//---------------------------------------------------------------------
	//! @class SceneGame
	//! @brief メインゲームシーンクラス
	//---------------------------------------------------------------------
	class SceneGame :
		//public Scene
		public DebugCameraScene
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
		//! @brief 遅延描画完了イベント
		void OnLateDrawFinish() override;

		//! @brief 終了処理
		void Exit() override;
		//! @brief ローディング状態を確認する
		//! @param init 初期化フラグ(確認したときに初期化処理も行うかどうか)
		//! @return ロード完了ならtrue、まだロード中ならfalse
		bool CheckForLoading(bool init = true);


		//! @brief 敵カウントを加算する
		void AddEnemyCount(u32 cnt = 1) { enemy_count += cnt; }
		//! @brief 敵カウントを減算する
		void SubtractEnemyCount(u32 cnt = 1);
		//! @brief 敵の総数を取得する
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

		ShadowMapObjectWP shadowmap = nullptr; //!< シャドウマップオブジェクト
		LightManagerWP light_manager = nullptr; //!< ライトマネージャー
		//! @brief ゲームタイマーを開始する
		void StartGameTimer() { is_game_timer_started = true; }
		//! @brief ゲームタイマーをリセットする
		void ResetGameTimer() { game_timer = 0.0f; }
		//! @brief ゲームタイマーを停止する
		void StopGameTimer() { is_game_timer_started = false; }
		//! @brief ゲームタイマーの値を取得する
		float GetGameTimer() const { return game_timer; }

		static float GetSEVolume() { return se_volume; }
		static float GetBGMVolume() { return bgm_volume; }

		AudioPlayerP audio_player; //!< オーディオプレイヤー

		//! シーンで使用するリソースをまとめて読み込む静的関数
		static void LoadResources();
	private:
		float game_timer = 0.0f; //!< ゲームタイマー
		bool is_game_timer_started = false; //!< タイマー動作中フラグ
		u32 enemy_count = 0; //!< 現在の敵の数
		static inline float bgm_volume = 1.0f; //!< BGM音量
		static inline float se_volume = 1.0f; //!< SE音量
	};
}
