//---------------------------------------------------------------------------
//! @file   PlayerClimbingState.h
//! @brief  プレイヤーのよじ登り状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Player;
	class PlayerClimbingState :
		public IState
	{
	public:
		//! @brief よじ登り状態を初期化し、完了後にアイドルへ遷移する条件を登録するコンストラクタ。
		//! @param owner_ この状態を所有するプレイヤーオブジェクト。
		PlayerClimbingState(Player* owner_);
		//! @brief 状態遷移直後によじ登りアニメーションと物理設定を初期化する処理。
		//! @param machine 状態機械本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に重力と物理シミュレーションを復元する終了処理。
		//! @param machine 状態機械本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行するよじ登り進行と位置補間処理。
		//! @param machine 状態機械本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief コリジョン侵入イベント（よじ登り中は処理なし）。
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {};
		//! @brief コリジョン継続イベント（よじ登り中は処理なし）。
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {};
		//! @brief コリジョン離脱イベント（よじ登り中は処理なし）。
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {};

		//! @brief よじ登り開始位置（Lerp の始点）。
		Vector3 climb_start_pos;
		//! @brief よじ登り終了位置（Lerp の終点）。
		Vector3 climb_end_pos;
	private:
		//! @brief 所有者となるプレイヤーオブジェクト。
		Player* owner_player;
		//! @brief 重力・キネマティック設定を変更する剛体。
		RigidBody* rb;
		//! @brief よじ登りモーションを制御するアニメータ。
		Animator* animator;
		//! @brief よじ登り完了までの残り時間。0 以下になると状態遷移する。
		float climb_time = 1.5f;
	};
}

