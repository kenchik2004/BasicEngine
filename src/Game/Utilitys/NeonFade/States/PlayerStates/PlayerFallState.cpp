//---------------------------------------------------------------------------
//! @file   PlayerFallState.cpp
//! @brief  PlayerFallStateの実装。プレイヤーの落下状態の処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "PlayerFallState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "System/Components/Animator.h"
#include "System/Components/Collider.h"

namespace NeonFade {
	//! @brief 落下状態を構築し、必要なコンポーネントへの参照を初期化する。
	//! @param player_ この状態を所有するプレイヤーオブジェクト。
	PlayerFallState::PlayerFallState(Player* player_) :
		IState(static_cast<GameObject*>(player_))
	{
		// 状態所有者のプレイヤー本体を保持する。
		player = player_;
		// 落下モーション再生に使用するアニメータを取得する。
		animator = player->animator.lock().get();
		// 落下速度設定に使用する剛体を取得する。
		rb = player->rb.lock().get();
	}
	//! @brief 落下状態へ入った瞬間の初期化処理。
	//! @param machine 状態機械本体。
	void PlayerFallState::OnEnter(IStateMachine* machine)
	{
		// 落下ループアニメーションをブレンド付きで再生する。
		animator->Play("fall_loop", true, 0.0f, 0.7f, true);
		// 下向きの初期速度を与えて落下感を強調する。
		rb->velocity.y = -10;
	}
	//! @brief 落下状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine 状態機械本体。
	void PlayerFallState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief 毎フレーム実行する更新処理（現在は空実装）。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerFallState::Update(IStateMachine* machine, float dt)
	{
	}
	//! @brief 地面接触継続イベント（現在は空実装）。
	//! @param machine 状態機械本体。
	//! @param hit_info 衝突情報。
	void PlayerFallState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{
	}
}