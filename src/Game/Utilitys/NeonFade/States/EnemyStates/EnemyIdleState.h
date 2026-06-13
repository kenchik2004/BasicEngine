//---------------------------------------------------------------------------
//! @file   EnemyIdleState.h
//! @brief  敵の待機状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {

	class Enemy;
	class EnemyIdleState :
		public IState
	{
	public:
		//! @brief 待機状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		EnemyIdleState(Enemy* owner_);
		//! @brief 状態遷移直後に待機アニメーションを開始する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する待機状態の更新処理（現在は空実装）。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief 衝突開始時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {}
		//! @brief 衝突継続時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief 衝突終了時のコールバック（このクラスでは未使用）。
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}

	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* enemy = nullptr;
		//! @brief 待機中の速度リセットに使用する剛体。
		RigidBody* rb = nullptr;
		//! @brief 待機モーションを制御するアニメータ。
		Animator* animator = nullptr;
	};

}
