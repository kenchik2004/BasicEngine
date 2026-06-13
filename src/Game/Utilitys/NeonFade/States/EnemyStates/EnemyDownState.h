//---------------------------------------------------------------------------
//! @file   EnemyDownState.h
//! @brief  敵のダウン状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {

	class Enemy;
	class EnemyDownState :
		public IState
	{
	public:
		//! @brief ダウン状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		EnemyDownState(Enemy* owner_);
		//! @brief 状態遷移直後にノックバック方向の判定と倒れアニメーションを開始する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前にコライダを通常状態に戻す終了処理。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行するヒットストップとノックバック適用の更新処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief 衝突開始時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {}
		//! @brief 衝突継続時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief 衝突終了時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief ダウン状態の経過時間を計測するタイマ。
		float exit_timer = 0;
		//! @brief ダウン状態を維持する最大時間。
		static constexpr float EXIT_TIME = 1.5f;
		//! @brief ヒットストップ継続時間の閾値。
		static constexpr float HITSTOP_TIME = 0.2f;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* enemy = nullptr;
		//! @brief ダウンモーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief ノックバック速度の適用に使用する剛体。
		RigidBody* rb;
		//! @brief ノックバック方向ベクトル。
		Vector3 knock_back_vec = { 0,0,0 };
		//! @brief ヒットストップ経過時間を計測するタイマ。
		float hit_stop_timer = 0;
		//! @brief ノックアウト時に再生する効果音クリップ。
		SafeSharedPtr<AudioClip> knockout_se;
		//! @brief ヒット時に再生する効果音クリップ。
		SafeSharedPtr<AudioClip> hit_se;
	};
}

