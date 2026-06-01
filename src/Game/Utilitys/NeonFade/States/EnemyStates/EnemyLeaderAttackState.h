//---------------------------------------------------------------------------
//! @file   EnemyLeaderAttackState.h
//! @brief  リーダー敵の攻撃状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyLeaderAttackState :
		public IState
	{
	public:
		//! @brief リーダー攻撃状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		//! @param player_ ターゲットとなるプレイヤーオブジェクトの弱参照。
		EnemyLeaderAttackState(Enemy* owner_, GameObjectWP player_);
		//! @brief 状態遷移直後に攻撃タイマをリセットして扇動アニメーションを開始する初期化処理。
		//! @param machine 状態機械本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine 状態機械本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行するプレイヤーへの向き更新とジャンプ扇動処理。
		//! @param machine 状態機械本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
	public:
		//! @brief 攻撃開始からの経過時間。
		float attack_timer = 0.0f;
		//! @brief 攻撃（扇動）を継続する最大時間。
		const float ATTACK_TIME = 3.0f;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* enemy;
		//! @brief ジャンプ力の付与に使用する剛体。
		RigidBody* rb;
		//! @brief 扇動モーションを制御するアニメータ。
		Animator* animator;
		//! @brief 向きを合わせる対象となるプレイヤーの弱参照。
		GameObjectWP player;
	};
}


