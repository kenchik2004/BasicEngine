//---------------------------------------------------------------------------
//! @file   PlayerDamageState.h
//! @brief  プレイヤーのダメージ状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Player;
	class PlayerDamageState :
		public IState
	{
	public:
		//! @brief ダメージ状態を初期化し、一定時間後にアイドルへ遷移する条件を登録するコンストラクタ。
		//! @param owner_ この状態を所有するプレイヤーオブジェクト。
		PlayerDamageState(Player* owner_);
		//! @brief 状態遷移直後にダメージアニメーションを高速再生する初期化処理。
		//! @param machine 状態機械本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前にアニメーション速度を通常に戻す終了処理。
		//! @param machine 状態機械本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム退場タイマを加算する更新処理。
		//! @param machine 状態機械本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief 退場タイマの現在値。
		float exit_timer = 0;
		//! @brief ダメージ状態を維持する時間。
		static constexpr float EXIT_TIME = 0.5f;
	private:
		//! @brief 所有者となるプレイヤーオブジェクト。
		Player* player = nullptr;
		//! @brief ダメージモーションを制御するアニメータ。
		Animator* animator = nullptr;
	};
}

