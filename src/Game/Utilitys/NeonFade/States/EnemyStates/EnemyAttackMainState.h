//---------------------------------------------------------------------------
//! @file   EnemyAttackMainState.h
//! @brief  敵の攻撃メイン状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyAttackMainState :
		public IState
	{
	public:
		//! @brief 攻撃メイン状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		EnemyAttackMainState(Enemy* owner_);
		//! @brief 状態遷移直後に攻撃処理を開始する初期化処理。
		//! @param machine 状態機械本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に攻撃判定を破棄する終了処理。
		//! @param machine 状態機械本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する攻撃状態更新処理。
		//! @param machine 状態機械本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief 攻撃判定が他コライダへ接触した際の処理。
		//! @param machine 状態機械本体。
		//! @param hit_info 接触情報。
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		//! @brief 攻撃開始からの経過時間。
		float attack_timer = 0;
		//! @brief 攻撃状態を維持する最大時間。
		static constexpr float ATTACK_TIME = 1.3f;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* owner_enemy = nullptr;
		//! @brief 攻撃モーションとアニメーションイベントを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief 攻撃中のみ有効化するヒットボックスコライダ。
		SafeSharedPtr<Collider> hit_box = nullptr;
	};
}
