//---------------------------------------------------------------------------
//! @file   EnemyAttackReadyState.h
//! @brief  敵の攻撃準備状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {

	class Enemy;
	class EnemyAttackReadyState :
		public IState
	{
	public:
		//! @brief 攻撃準備状態を初期化するコンストラクタ。
		//! @param enemy_ この状態を所有する敵オブジェクト。
		EnemyAttackReadyState(Enemy* enemy_);
		//! @brief 状態遷移直後に準備処理を開始する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する準備状態の更新処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief 状態遷移直前に実行する終了処理。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* owner_enemy = nullptr;
		//! @brief 歩行モーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief 移動と向き変更に使用する剛体。
		RigidBody* rb = nullptr;
		//! @brief 追跡対象（プレイヤー）の弱参照。
		GameObjectWP target = nullptr;
		//! @brief 攻撃チャージへ遷移するフラグ。
		bool next = false;
		//! @brief 最大準備待機時間の基準値。
		static constexpr float max_ready_time = 4.0f;
		//! @brief 攻撃準備の残り時間を計測するタイマ。
		float ready_timer = 0.0f;
		//! @brief 攻撃開始を判断する目標との接近距離（毎回乱数で決定）。
		float random_attack_distance = 0.0f;
	};

}
