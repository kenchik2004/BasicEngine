//---------------------------------------------------------------------------
//! @file   PlayerIdleState.cpp
//! @brief  PlayerIdleStateクラスの実装
//---------------------------------------------------------------------------
#include "PlayerIdleState.h"

#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"

namespace NeonFade
{
	//! @brief 待機状態を構築し、必要なコンポーネントへの参照を初期化する。
	//! @param owner_ この状態を所有するプレイヤーオブジェクト。
	PlayerIdleState::PlayerIdleState(Player* owner_) :
		IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者のプレイヤー本体を保持する。
		player = owner_;
		// 慣性打ち消しに使用する剛体を取得する。
		rb = player->rb.lock().get();
		// アイドルモーション再生に使用するアニメータを取得する。
		animator = player->animator.lock().get();
	}
	//! @brief 待機状態へ入った瞬間の初期化処理。
	//! @param machine 状態機械本体。
	void PlayerIdleState::OnEnter(IStateMachine* machine)
	{
		// アイドルアニメーションをループ再生して待機姿勢を表現する。
		animator->Play("idle", true, 0.0f, 0.1f, false);
	}
	//! @brief 待機状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine 状態機械本体。
	void PlayerIdleState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief 慣性の打ち消しとキー/スティック入力の検出を行う更新処理。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerIdleState::Update(IStateMachine* machine, float dt)
	{
		// 現在の速度の逆方向に弱い力を加えて水平方向の慣性を打ち消す。
		Vector3 force = rb->velocity * -0.5f;
		force.y = 0;
		rb->AddForce(force, ForceMode::Impulse); // 慣性を打ち消す
		bool input_exist = false;
		//-------------------------------------------------
		// 左スティックの入力値を取得する。
		Vector2 pad_left = Input::GetPadLeftStick(0);

		if (pad_left.magnitudeSquared() >= FLT_EPSILON) {
			// スティック入力が存在する場合は入力フラグを立てる。
			input_exist = true;
		}
		// WASDキーのいずれかが押されている場合も入力フラグを立てる。
		if (Input::GetKey(KeyCode::W) || Input::GetKey(KeyCode::S) || Input::GetKey(KeyCode::A) || Input::GetKey(KeyCode::D))
			input_exist = true;
		auto current_velocity = rb->velocity;
		//rb->velocity = { 0,current_velocity.y,0 };
	}
	//! @brief 地面接触継続イベント（現在は空実装）。
	//! @param machine 状態機械本体。
	//! @param hit_info 衝突情報。
	void PlayerIdleState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{
	}
}