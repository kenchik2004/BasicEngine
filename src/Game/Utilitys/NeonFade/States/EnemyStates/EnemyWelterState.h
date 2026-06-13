//---------------------------------------------------------------------------
//! @file   EnemyWelterState.h
//! @brief  敵のよろめき状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyWelterState :
		public IState
	{
	public:
		//! @brief よろめき状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		EnemyWelterState(Enemy* owner_);
		//! @brief 状態遷移直後にタイマをリセットして逃走アニメーションを開始する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行するランダム回転と前進速度の設定処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* enemy = nullptr;
		//! @brief 前進速度の設定に使用する剛体。
		RigidBody* rb = nullptr;
		//! @brief よろめきモーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief よろめき開始からの経過時間。
		float timer = 0.0f;
		//! @brief よろめき状態を維持する最大時間。
		static constexpr float WELTER_TIME = 3.0f;
	};
}

