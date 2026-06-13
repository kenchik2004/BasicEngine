//---------------------------------------------------------------------------
//! @file   EnemyMoveState.h
//! @brief  敵の移動状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyMoveState :
		public IState
	{
	public:
		//! @brief 移動状態を初期化するコンストラクタ。
		//! @param enemy_ この状態を所有する敵オブジェクト。
		EnemyMoveState(Enemy* enemy_);
		//! @brief 状態遷移直後に歩行アニメーションを開始する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する移動方向への向き更新と速度設定処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* owner_enemy = nullptr;
		//! @brief 歩行モーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief 移動速度の設定に使用する剛体。
		RigidBody* rb = nullptr;
	};

}
