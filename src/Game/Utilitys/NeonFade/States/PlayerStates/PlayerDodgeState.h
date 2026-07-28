//---------------------------------------------------------------------------
//! @file   PlayerDodgeState.h
//! @brief  プレイヤーの回避状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Player;
	class PlayerDodgeState :
		public IState
	{
	public:
		//! @brief 回避状態を初期化し、回避終了後の遷移条件を登録するコンストラクタ。
		//! @param owner_ この状態を所有するプレイヤーオブジェクト。
		PlayerDodgeState(Player* owner_);
		//! @brief 状態遷移直後に回避アニメーションと速度ベクトルを設定する初期化処理。
		//! @param machine ステートマシン本体。
		virtual void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine ステートマシン本体。
		virtual void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム回避タイマを加算する更新処理。
		//! @param machine ステートマシン本体。
		//! @param dt 前フレームからの経過時間。
		virtual void Update(IStateMachine* machine, float dt) override;
		//! @brief 回避開始からの経過時間。
		float dodge_timer = 0;
		//! @brief 回避状態を維持する時間。
		static constexpr float DODGE_TIME = 0.2f;
		//! @brief 無敵フレームの継続時間。
		static constexpr float I_FRAME_TIME = 0.1f;
		//! @brief 回避時に付与する速度の大きさ。
		static constexpr float DODGE_SPEED = 90.0f;
	private:
		//! @brief 所有者となるプレイヤーオブジェクト。
		Player* player = nullptr;
		//! @brief 回避モーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief 回避速度の付与に使用する剛体。
		RigidBody* rb = nullptr;
	};
}

