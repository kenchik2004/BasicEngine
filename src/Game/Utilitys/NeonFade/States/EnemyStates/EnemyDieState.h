//---------------------------------------------------------------------------
//! @file   EnemyDieState.h
//! @brief  敵の死亡状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Enemy;
	class EnemyDieState :
		public IState
	{
	public:
		//! @brief 死亡状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		EnemyDieState(Enemy* owner_);
		//! @brief デストラクタ。インスタンス数をデクリメントする。
		~EnemyDieState() override;
		//! @brief 状態遷移直後に物理コンポーネント削除と死亡マテリアル適用を行う初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する死亡演出の更新処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* owner_enemy = nullptr;
		//! @brief 死亡モーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief 死亡マテリアルを適用するモデルレンダラ。
		ModelRenderer* model = nullptr;
		//! @brief 死亡演出開始からの経過時間を計測するタイマ。
		float exit_timer = 0;
		//! @brief 死亡演出が完了するまでの時間。
		static constexpr float EXIT_TIME = 1.0f;
	};
}

