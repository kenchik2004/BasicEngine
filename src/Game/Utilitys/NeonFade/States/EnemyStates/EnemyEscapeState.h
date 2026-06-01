//---------------------------------------------------------------------------
//! @file   EnemyEscapeState.h
//! @brief  敵の逃走状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Enemy;
	class EnemyEscapeState :
		public IState
	{
	public:
		//! @brief 逃走状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有する敵オブジェクト。
		//! @param leader_ 追従先リーダーオブジェクトの弱参照（なければ単独逃走）。
		EnemyEscapeState(Enemy* owner_, SafeWeakPtr<Object> leader_);
		//! @brief 状態遷移直後に逃走タイマとアニメーションを初期化する処理。
		//! @param machine 状態機械本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine 状態機械本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する逃走移動と終了判定の更新処理。
		//! @param machine 状態機械本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
	private:
		//! @brief 所有者となる敵オブジェクト。
		Enemy* enemy = nullptr;
		//! @brief リーダーオブジェクトの弱参照（リーダーが存在しない場合は null）。
		SafeWeakPtr<Object> leader = nullptr;
		//! @brief 逃走移動に使用する剛体。
		RigidBody* rb = nullptr;
		//! @brief 逃走モーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief 追跡・回避の基準となるプレイヤーオブジェクト。
		GameObject* player = nullptr;
		//! @brief 逃走開始からの残り時間を計測するタイマ。
		float escape_timer = 0.0f;
		//! @brief 逃走を継続する最大時間。
		static constexpr float ESCAPE_TIME = 5.0f;
		//! @brief 逃走終了をアイドル遷移条件へ通知するフラグ。
		bool exit_flag = false;
	};
}

