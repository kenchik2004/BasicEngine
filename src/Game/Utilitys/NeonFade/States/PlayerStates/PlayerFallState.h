//---------------------------------------------------------------------------
//! @file   PlayerFallState.h
//! @brief  プレイヤーの落下状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

class RigidBody;
class Animator;
namespace NeonFade {
	class Player;

	class PlayerFallState :
		public IState
	{
	public:
		//! @brief 落下状態を初期化するコンストラクタ。
		//! @param player_ この状態を所有するプレイヤーオブジェクト。
		PlayerFallState(Player* player_);
		//! @brief 状態遷移直後に落下ループアニメーションと初期落下速度を設定する初期化処理。
		//! @param machine 状態機械本体。
		void OnEnter(IStateMachine* machine) override;
		//! @brief 状態遷移直前に実行する終了処理（現在は空実装）。
		//! @param machine 状態機械本体。
		void OnExit(IStateMachine* machine) override;
		//! @brief 毎フレーム実行する更新処理（現在は空実装）。
		//! @param machine 状態機械本体。
		//! @param dt 前フレームからの経過時間。
		void Update(IStateMachine* machine, float dt) override;
		//! @brief コリジョン侵入イベント（落下状態では処理なし）。
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override {}
		//! @brief 地面接触継続イベント（現在は空実装）。
		//! @param machine 状態機械本体。
		//! @param hit_info 衝突情報。
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override;
		//! @brief コリジョン離脱イベント（落下状態では処理なし）。
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
	private:
		//! @brief 所有者となるプレイヤーオブジェクト。
		Player* player = nullptr;
		//! @brief 落下モーションを制御するアニメータ。
		Animator* animator = nullptr;
		//! @brief 落下速度設定に使用する剛体。
		RigidBody* rb = nullptr;
	};
}

