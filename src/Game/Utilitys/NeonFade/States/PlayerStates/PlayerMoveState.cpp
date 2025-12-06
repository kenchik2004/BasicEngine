#include "precompile.h"
#include "PlayerMoveState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "System/Components/RigidBody.h"
#include "System/Components/Animator.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "Game/Components/PlayerController.h"
#include "System/Objects/CameraObject.h"
#include "System/Components/Collider.h"

Vector3 Slerp(const Vector3& from, const Vector3& to, float t)
{
	// 正規化（方向ベクトルを前提とする）
	Vector3 v0 = from.getNormalized();
	Vector3 v1 = to.getNormalized();

	// 内積を取得
	float dot = physx::PxClamp(v0.dot(v1), -1.0f, 1.0f);

	// tよりも近い場合はターゲットを返す
	if (dot >= 1 - t)
		return v1;

	// 角度を算出（ラジアン）
	float theta = acosf(dot);




	// sin(θ)を利用して補間（球面線形補間）
	float sinTheta = sinf(theta);
	float w1 = sinf((1.0f - t) * theta) / sinTheta;
	float w2 = sinf(t * theta) / sinTheta;

	// 補間
	Vector3 result = (v0 * w1) + (v1 * w2);
	return result.getNormalized();
}
namespace NeonFade
{
	PlayerMoveState::PlayerMoveState(Player* owner_) :
		IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		rb = player->rb.lock().get();
		animator = player->animator.lock().get();
	}
	void PlayerMoveState::OnEnter(IStateMachine* machine)
	{

		if (sprint)
			animator->PlayIfNoSame("sprint", true, 0.0f, 0.2f, false);
		else
			animator->PlayIfNoSame("run", true, 0.0f, 0.2f, false);
		fall_timer = 0.0f;	// タイマーリセット
	}
	void PlayerMoveState::OnExit(IStateMachine* machine)
	{
		fall_timer = 0.0f;	// タイマーリセット
	}
	void PlayerMoveState::Update(IStateMachine* machine, float dt)
	{
		fall_timer += dt;
		if (fall_timer >= max_fall_time)
		{
			machine->ChangeState("fall");
			return;
		}
		Transform* pl_trns = player->transform.get();

		Vector3 input = static_cast<PlayerStateMachine*>(machine)->move_input;
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

		if (sprint)
			input = input * 45;
		else
			input = input * 25;
		input_dbg = input.getNormalized();
		input.y = rb->velocity.y;	// 重力成分を維持
		rb->velocity = input;
	}
	void PlayerMoveState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{
		fall_timer = 0.0f;// 地面に接触している間は常にタイマーをリセット
	}
	void PlayerMoveState::OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info)
	{
		fall_timer = 0.0f;// 地面に接触している間は常にタイマーをリセット
	}
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
