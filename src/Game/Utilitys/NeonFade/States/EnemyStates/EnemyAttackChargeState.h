//---------------------------------------------------------------------------
//! @file   EnemyAttackChargeState.h
//! @brief  敵の攻撃チャージ状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {
	class Enemy;
	class EnemyAttackChargeState :
		public IState
	{
	public:
		//! @brief 攻撃チャージ状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		EnemyAttackChargeState(Enemy* owner_);
		//! @brief 状態遷移直後に実行する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する更新処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief チャージ開始からの経過時間。
		float charge_timer = 0;
		//! @brief 攻撃本体状態へ遷移するまでのチャージ時間。
		static constexpr float CHARGE_TIME = 1.0f;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* owner_enemy = nullptr;
		//! @brief 攻撃チャージ演出を制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief チャージ中の移動停止と向き調整に使用する剛体。
		RigidBody* rb = nullptr;
	};
}
