//---------------------------------------------------------------------------
//! @file   PlayerJumpState.cpp
//! @brief  PlayerJumpStateの実装。プレイヤーのジャンプ状態の処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "PlayerJumpState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "System/Components/Collider.h"
#include "System/Components/RigidBody.h"
#include "System/Components/Animator.h"
#include "System/Objects/CameraObject.h"

namespace NeonFade
{

	//! @brief ジャンプ状態を構築し、必要なコンポーネントへの参照を初期化する。
	//! @param owner_ この状態を所有するプレイヤーオブジェクト。
	PlayerJumpState::PlayerJumpState(Player* owner_)
		: IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者のプレイヤー本体を保持する。
		player = owner_;
		// 速度設定に使用する剛体を取得する。
		rb = player->rb.lock().get();
		// ジャンプモーション再生に使用するアニメータを取得する。
		animator = player->animator.lock().get();

	}

	//! @brief ジャンプ状態へ入った瞬間の初期化処理。
	//! @param machine ステートマシン本体。
	void PlayerJumpState::OnEnter(IStateMachine* machine)
	{
		// アニメーション速度を通常に戻す。
		animator->anim_speed = 1.0f;
		// ジャンプアニメーションを 1 回再生する。
		animator->Play("jump", false, 0.0f, 0.3f, true);
		// 上方向速度を与えてジャンプを開始する。
		rb->velocity = rb->velocity.getNormalized() * JUMP_FORCE;
		rb->velocity.y = JUMP_FORCE;
		// ジャンプタイマを初期化する。
		jump_timer = 0.0f;
	}

	//! @brief ジャンプ状態を抜ける際に重力を有効化する終了処理。
	//! @param machine ステートマシン本体。
	void PlayerJumpState::OnExit(IStateMachine* machine)
	{
		// 重力を有効に戻して着地後の落下を正常に処理する。
		rb->use_gravity = true;
	}

	//! @brief 空中での向き更新と水平移動速度設定を行う更新処理。
	//! @param machine ステートマシン本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerJumpState::Update(IStateMachine* machine, float dt)
	{
		// ステートマシンから現在の移動入力を取得する。
		auto pl_machine = static_cast<PlayerStateMachine*>(machine);

		Vector3 input = static_cast<PlayerStateMachine*>(machine)->move_input;
		// プレイヤーのトランスフォームを取得する。
		Transform* pl_trns = player->transform.get();
		// 現在の前方ベクトルを取得する。
		Vector3 forward = pl_trns->AxisZ();

		// 前方と移動入力の内積を求める。
		float dot = forward.dot(input);

		// 内積を [-1, 1] にクランプしてアークコサインの定義域を保証する。
		dot = physx::PxClamp(dot, -1.0f, 1.0f);
		// 内積から角度差（度）を算出する。
		float angle = RAD2DEG(acosf(dot));
		// 角速度を算出して空中での向き変更速度を決定する。
		float rot_angle = 720 / angle;
		// 後方入力の場合は向き変更をより素早くする。
		if (dot < 0.0f)
			rot_angle *= 5;
		// 算出した角速度で前方を入力方向へ Lerp する。
		pl_trns->SetAxisZ(Lerp(pl_trns->AxisZ(), input, rot_angle * Time::DeltaTime()));

		// 水平速度を設定する（入力方向 × 一定速度）。
		input = input * JUMP_FORCE;
		input.y = rb->velocity.y;	// 重力成分を維持
		// 垂直速度を保持したまま水平速度を上書きする。
		rb->velocity = input;
		// ジャンプタイマを加算する。
		jump_timer += dt;
	}

	//! @brief 地面接触継続イベント（現在は空実装）。
	//! @param machine ステートマシン本体。
	//! @param hit_info 衝突情報。
	void PlayerJumpState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{

	}


}