//---------------------------------------------------------------------------
//! @file   PlayerClimbingState.cpp
//! @brief  PlayerClimbingStateの実装。プレイヤーのよじ登り状態の処理を行う
//---------------------------------------------------------------------------
#include "PlayerClimbingState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
namespace NeonFade
{
	//! @brief よじ登り状態を構築し、残り時間が 0 以下になったらアイドルへ遷移する条件を登録する。
	//! @param owner_ この状態を所有するプレイヤーオブジェクト。
	PlayerClimbingState::PlayerClimbingState(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者のプレイヤー本体を保持する。
		owner_player = owner_;
		// 重力・キネマティック設定変更に使用する剛体を取得する。
		rb = owner_player->rb.lock().get();
		// よじ登りモーション再生に使用するアニメータを取得する。
		animator = owner_player->animator.lock().get();
		// よじ登り残り時間が 0 以下になったらアイドルへ遷移させる条件式。
		std::function<bool()> exit_request = [this]() {
			return climb_time <= 0.0f;
			};

		// 遷移先「idle」とよじ登り完了条件を優先度 0 で登録する。
		RegisterChangeRequest("idle", exit_request, 0);
	}
	//! @brief よじ登り状態へ入った瞬間の初期化処理。
	//! @param machine 状態機械本体。
	void PlayerClimbingState::OnEnter(IStateMachine* machine)
	{
		// よじ登り残り時間を初期化する。
		climb_time = 1.5f;
		// 速度をゼロにして意図しない移動を防ぐ。
		rb->velocity = Vector3(0, 0, 0);
		// 重力を無効にしてよじ登り中に落下しないようにする。
		rb->use_gravity = false;
		// キネマティックモードにして物理演算による位置変化を無効化する。
		rb->is_kinematic = true;
		// よじ登りアニメーションをブレンド付きで再生する。
		animator->Play("climb", false, 0.0f, 0.2f, true);
		// 状態機械からよじ登り開始・終了位置を取得して補間に備える。
		auto pl_machine = static_cast<PlayerStateMachine*>(machine);
		climb_start_pos = pl_machine->climb_start_pos;
		climb_end_pos = pl_machine->climb_end_pos;
	}
	//! @brief よじ登り状態を抜ける際に重力と物理シミュレーションを復元する終了処理。
	//! @param machine 状態機械本体。
	void PlayerClimbingState::OnExit(IStateMachine* machine)
	{
		// 重力を有効に戻してよじ登り完了後の落下を許可する。
		rb->use_gravity = true;
		// キネマティックモードを解除して通常の物理演算を再開する。
		rb->is_kinematic = false;
	}
	//! @brief よじ登りの進行と位置補間を行う更新処理。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerClimbingState::Update(IStateMachine* machine, float dt)
	{
		// リアルタイムのデルタタイムで残り時間を減算する。
		climb_time -= Time::RealDeltaTime();
		if (climb_time < 1.0f) {
			// 残り時間が 1 秒未満になったら開始位置から終了位置へ線形補間で移動する。
			owner_player->transform->position = Lerp(climb_start_pos, climb_end_pos, (1.0f - climb_time));
		}
	}
}
