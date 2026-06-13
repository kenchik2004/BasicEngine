//---------------------------------------------------------------------------
//! @file   PlayerDamageState.cpp
//! @brief  PlayerDamageStateの実装。プレイヤーのダメージ状態の処理を行う
//---------------------------------------------------------------------------
#include "PlayerDamageState.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade
{
	//! @brief ダメージ状態を構築し、一定時間後にアイドルへ戻る遷移条件を登録する。
	//! @param owner_ この状態を所有するプレイヤーオブジェクト。
	PlayerDamageState::PlayerDamageState(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者のプレイヤー本体を保持する。
		player = owner_;
		// ダメージモーション再生に使用するアニメータを取得する。
		animator = player->animator.lock().get();
		
		// 退場タイマが上限を超えたらアイドルへ遷移させる条件式。
		std::function default_change = [this]() {
			return exit_timer >= EXIT_TIME;
			};
		// 遷移先「idle」と時間終了条件を優先度 1 で登録する。
		RegisterChangeRequest("idle", default_change, 1);
	}
	//! @brief ダメージ状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void PlayerDamageState::OnEnter(IStateMachine* machine)
	{
		// アニメーション速度を 2 倍にしてダメージリアクションを短く見せる。
		animator->anim_speed = 2.0f;
		// 同一アニメーションが再生中でない場合のみダメージモーションを再生する。
		animator->PlayIfNoSame("player_damage");
		// 退場タイマを初期化して経過時間の計測を開始する。
		exit_timer = 0;
		//rb->velocity = { 0, 0, 0 };
	}
	//! @brief ダメージ状態を抜ける際にアニメーション速度を通常に戻す終了処理。
	//! @param machine ステートマシン本体。
	void PlayerDamageState::OnExit(IStateMachine* machine)
	{
		//animator->Stop();
		// アニメーション速度を通常の 1.0 に戻す。
		animator->anim_speed = 1.0f;
	}
	//! @brief 退場タイマを加算する更新処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerDamageState::Update(IStateMachine* machine, float dt)
	{
		// 退場タイマを加算して遷移条件の判定に使用する。
		exit_timer += dt;
	}
}