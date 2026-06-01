//---------------------------------------------------------------------------
//! @file   PlayerMoveState.cpp
//! @brief  PlayerMoveStateの実装。プレイヤーの移動状態の処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "PlayerMoveState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "System/Components/RigidBody.h"
#include "System/Components/Animator.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "Game/Components/PlayerController.h"
#include "System/Objects/CameraObject.h"
#include "System/Components/Collider.h"

namespace NeonFade
{
	//! @brief 移動状態を構築し、必要なコンポーネント参照を初期化する。
	//! @param owner_ この状態を所有するプレイヤーオブジェクト。
	PlayerMoveState::PlayerMoveState(Player* owner_) :
		IState(static_cast<GameObject*>(owner_))
	{
		// 状態所有者のプレイヤー本体を保持する。
		player = owner_;
		// 速度制御に使用する剛体を取得する。
		rb = player->rb.lock().get();
		// 走行/ダッシュモーション再生に使用するアニメータを取得する。
		animator = player->animator.lock().get();
	}
	//! @brief 移動状態に入った際にアニメーションと落下判定タイマを初期化する。
	//! @param machine 状態機械本体。
	void PlayerMoveState::OnEnter(IStateMachine* machine)
	{

		// ダッシュ中かどうかで再生する移動モーションを切り替える。
		if (sprint)
			animator->PlayIfNoSame("sprint", true, 0.0f, 0.2f, false);
		else
			animator->PlayIfNoSame("run", true, 0.0f, 0.2f, false);
		fall_timer = 0.0f;	// タイマーリセット
	}
	//! @brief 移動状態を抜ける際に落下判定タイマをリセットする。
	//! @param machine 状態機械本体。
	void PlayerMoveState::OnExit(IStateMachine* machine)
	{
		fall_timer = 0.0f;	// タイマーリセット
	}
	//! @brief 入力に応じて向き・移動速度を更新し、落下判定を行う。
	//! @param machine 状態機械本体。
	//! @param dt 前フレームからの経過時間。
	void PlayerMoveState::Update(IStateMachine* machine, float dt)
	{
		// 空中にいる時間を計測し、閾値を超えたら落下状態へ遷移する。
		fall_timer += dt;
		if (fall_timer >= max_fall_time)
		{
			machine->ChangeState("fall");
			return;
		}
		Transform* pl_trns = player->transform.get();

		// 状態機械から渡された移動入力ベクトルを取得する。
		Vector3 input = static_cast<PlayerStateMachine*>(machine)->move_input;
		// Shiftキーまたは左スティック押し込みでダッシュ状態をトグルする。
		if (Input::GetKeyDown(KeyCode::LShift) || Input::GetPadButtonDown(0, PadButton::LeftStick)) {
			sprint = !sprint;
			if (sprint) {
				animator->PlayIfNoSame("sprint", true, 0.0f, 0.2f, false);
				animator->anim_speed = 1.5f;
			}
			else {
				animator->PlayIfNoSame("run", true, 0.0f, 0.2f, false);
				animator->anim_speed = 1.0f;
			}
		}

		// 入力方向へ正規化し、プレイヤー前方からの角度差を算出して向きを補間する。
		input = input.getNormalized();
		Vector3 forward = pl_trns->AxisZ();

		float dot = forward.dot(input.getNormalized());


		dot = physx::PxClamp(dot, -1.0f, 1.0f);
		float angle = RAD2DEG(acosf(dot));
		float rot_angle = (360 * 4) / angle;
		//if (dot < 0.0f)
		//	rot_angle *= 5;
		if (sprint)
			rot_angle *= 1.5f;
		pl_trns->SetAxisZ(Slerp(forward, input.getNormalized(), rot_angle * dt));

		// ダッシュ状態に応じて移動速度係数を変更する。
		if (sprint)
			input = input * 45;
		else
			input = input * 25;
		input_dbg = input.getNormalized();
		input.y = rb->velocity.y;	// 重力成分を維持
		// 水平方向の目標速度を剛体へ反映する。
		rb->velocity = input;
	}
	//! @brief 接地中は落下判定タイマをリセットする。
	//! @param machine 状態機械本体。
	//! @param hit_info 衝突情報。
	void PlayerMoveState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{
		fall_timer = 0.0f;// 地面に接触している間は常にタイマーをリセット
	}
	//! @brief 接地判定離脱時に落下判定タイマをリセットする。
	//! @param machine 状態機械本体。
	//! @param hit_info 衝突情報。
	void PlayerMoveState::OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info)
	{
		fall_timer = 0.0f;// 地面に接触している間は常にタイマーをリセット
	}
	//! @brief 入力方向・前方方向・補間方向をラインで可視化するデバッグ描画。
	void PlayerMoveState::DebugDraw()
	{
		Vector3 line_start = cast(player->transform->position + Vector3(0, 3, 0));
		Vector3 forward = player->transform->AxisZ();

		float dot = forward.dot(input_dbg);


		dot = physx::PxClamp(dot, -1.0f, 1.0f);
		float angle = (acosf(dot));
		float rot_angle = DEG2RAD(360 * 8 * Time::DrawDeltaTime()) / angle;
		Vector3 slerped = Slerp(forward, input_dbg, rot_angle);
		DrawLine3D(cast(line_start), cast(line_start + input_dbg * 2), Color::YELLOW);
		DrawLine3D(cast(line_start), cast(line_start - player->transform->AxisZ() * 2), Color::RED);
		DrawLine3D(cast(line_start), cast(line_start - slerped * 2), Color::BLUE);
	}
}
