//---------------------------------------------------------------------------
//! @file   EnemyDamageState.h
//! @brief  敵のダメージ状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {

	class Enemy;
	class EnemyDamageState :
		public IState
	{
	public:
		//! @brief ダメージ状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		EnemyDamageState(Enemy* owner_);
		//! @brief 状態遷移直後にダメージ演出を開始する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行するダメージ状態の更新処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief 衝突開始時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {}
		//! @brief 衝突継続時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief 衝突終了時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief ヒットストップ継続時間の閾値。
		static constexpr float HITSTOP_TIME = 0.2f;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* enemy = nullptr;
		//! @brief ダメージモーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief ヒットストップ中の速度制御に使用する剛体。
		RigidBody* rb;
		//! @brief ヒットストップ経過時間を計測するタイマ。
		float hit_stop_timer = 0;
		//! @brief ヒット時に再生する効果音クリップ。
		SafeSharedPtr<AudioClip> hit_se;
	};

}
