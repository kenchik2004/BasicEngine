//---------------------------------------------------------------------------
//! @file   EnemyLeaderStepBackState.h
//! @brief  リーダー敵の後退状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyLeaderStepBackState :
		public IState
	{
	public:
		//! @brief リーダー後退状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		//! @param player_ 向きを合わせる対象となるプレイヤーオブジェクトの弱参照。
		EnemyLeaderStepBackState(Enemy* owner_, GameObjectWP player_);
		//! @brief 状態遷移直後にタイマをリセットし後退アニメーションとプレイヤーへの向きを設定する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する後退ジャンプのタイミング制御処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
	public:
		//! @brief 後退開始からの経過時間。
		float step_back_timer = 0.0f;
		//! @brief 後退状態を維持する最大時間。
		const float STEP_BACK_TIME = 2.0f;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* enemy;
		//! @brief 後退ジャンプ力の付与に使用する剛体。
		RigidBody* rb;
		//! @brief 後退モーションを制御するアニメータ。
		Animator* animator;
		//! @brief 向きを合わせる対象となるプレイヤーの弱参照。
		GameObjectWP player;
		//! @brief 後退ジャンプを一度だけ実行したかを管理するフラグ。
		bool jumped = false;
	};
}

