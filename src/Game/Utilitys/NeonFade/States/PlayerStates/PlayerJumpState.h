//---------------------------------------------------------------------------
//! @file   PlayerJumpState.h
//! @brief  プレイヤーのジャンプ状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

class RigidBody;
class Animator;
namespace NeonFade
{
	class Player;
	class PlayerJumpState :
		public IState
	{
	public:
		//! @brief ジャンプ状態を初期化するコンストラクタ。
		//! @param owner_ この状態を所有するプレイヤーオブジェクト。
		PlayerJumpState(Player* owner_);
		//! @brief 状態遷移直後にジャンプアニメーションと上方向速度を設定する初期化処理。
		//! @param machine ステートマシン本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に重力を有効化する終了処理。
		//! @param machine ステートマシン本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する空中移動と向き更新処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief コリジョン侵入イベント（ジャンプ中は処理なし）。
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief 地面接触継続イベント（現在は空実装）。
		//! @param machine ステートマシン本体。
		//! @param hit_info 衝突情報。
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override;
		//! @brief コリジョン離脱イベント（ジャンプ中は処理なし）。
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief ジャンプ開始からの経過時間。
		float jump_timer = 0.0f;
		//! @brief ジャンプ状態の最大維持時間。
		static constexpr float JUMP_TIME = 0.4f;
	private:
		//! @brief 所有者となるプレイヤーオブジェクト。
		Player* player = nullptr;
		//! @brief 速度設定に使用する剛体。
		RigidBody* rb = nullptr;
		//! @brief ジャンプモーションを制御するアニメータ。
		Animator* animator = nullptr;
	};
}

