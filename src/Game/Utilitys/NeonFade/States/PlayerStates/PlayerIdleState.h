//---------------------------------------------------------------------------
//! @file   PlayerIdleState.h
//! @brief  プレイヤーの待機状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

class RigidBody;
class Animator;
namespace NeonFade
{
	class Player;
	class PlayerIdleState :
		public IState
	{
	public:
		//! @brief 待機状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有するプレイヤーオブジェクト。
		PlayerIdleState(Player* owner_);
		//! @brief 状態遷移直後にアイドルアニメーションを開始する初期化処理。
		//! @param machine 状態機械本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine 状態機械本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する慣性打ち消しと入力検出処理。
		//! @param machine 状態機械本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief コリジョン侵入イベント（待機状態では処理なし）。
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief コリジョン継続イベント。
		//! @param machine 状態機械本体。
		//! @param hit_info 衝突情報。
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override;
		//! @brief コリジョン離脱イベント（待機状態では処理なし）。
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}

	private:
		//! @brief 所有者となるプレイヤーオブジェクト。
		Player* player = nullptr;
		//! @brief 慣性打ち消しに使用する剛体。
		RigidBody* rb = nullptr;
		//! @brief アイドルモーションを制御するアニメータ。
		Animator* animator = nullptr;
	};
}

