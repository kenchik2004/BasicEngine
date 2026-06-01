//---------------------------------------------------------------------------
//! @file   PlayerDodgeState.cpp
//! @brief  PlayerDodgeStateの実装。プレイヤーの回避状態の処理を行う
//---------------------------------------------------------------------------
#include "PlayerDodgeState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/PlayerController.h"

namespace NeonFade
{
	//! @brief 回避状態を構築し、回避タイマ終了後に落下状態へ遷移する条件を登録する。
	//! @param owner_ この状態を所有するプレイヤーオブジェクト。
	PlayerDodgeState::PlayerDodgeState(Player* owner_) :
		IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者のプレイヤー本体を保持する。
		player = owner_;
		// 回避モーション再生に使用するアニメータを取得する。
		animator = player->animator.lock().get();
		// 回避速度付与に使用する剛体を取得する。
		rb = player->rb.lock().get();
		// 回避タイマが上限を超えたら落下状態へ遷移させる条件式。
		std::function<bool()> dodge_to_idle = [this]() {
			return dodge_timer >= DODGE_TIME;
			};
		// 遷移先「fall」と時間終了条件を優先度 0 で登録する。
		RegisterChangeRequest("fall", dodge_to_idle, 0);
	}
	//! @brief 回避状態へ入った瞬間の初期化処理。
	//! @param machine 状態機械本体。
	void PlayerDodgeState::OnEnter(IStateMachine* machine)
	{
		// バック転アニメーションをループなしで再生して回避モーションを表現する。
		animator->Play("back_flip", false, 0.0f, 0.05f, true);
		// 回避タイマを初期化する。
		dodge_timer = 0.0f;
		// スティック入力から回避方向を取得する。
		Vector3 dodge_dir = static_cast<PlayerStateMachine*>(machine)->move_input;
		if (dodge_dir.magnitudeSquared() <= 1e-6f * 1e-6f) {
			// 入力がない場合は現在の前方逆方向（後方）へ回避する。
			dodge_dir = -player->transform->AxisZ();
		}
		else {
			// 入力がある場合は正規化して回避方向を決定する。
			dodge_dir.normalize();
		}
		// 垂直成分を除去して水平面上の回避方向のみ使用する。
		dodge_dir = ProjectOnPlane(dodge_dir, { 0,1,0 });

		// 回避方向の逆を向いて後退感を演出する。
		player->transform->SetAxisZ(-dodge_dir.getNormalized(), { 0,1,0 });
		// 回避速度を乗算して水平方向の速度を設定する。
		dodge_dir *= DODGE_SPEED;
		dodge_dir.y = 20; // 少しジャンプさせる
		// 算出した速度ベクトルを剛体に適用する。
		rb->velocity = dodge_dir;
	}
	//! @brief 回避状態を抜ける際の終了処理（現在は空実装）。
	//! @param machine 状態機械本体。
	void PlayerDodgeState::OnExit(IStateMachine* machine)
	{
	}
	//! @brief 回避タイマを加算する更新処理。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerDodgeState::Update(IStateMachine* machine, float dt)
	{
		// 回避経過時間を加算して遷移条件の判定に使用する。
		dodge_timer += dt;
	}
}