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

	PlayerJumpState::PlayerJumpState(Player* owner_)
		: IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		rb = player->rb.lock().get();
		animator = player->animator.lock().get();

	}

	void PlayerJumpState::OnEnter(IStateMachine* machine)
	{
		animator->Play("jump", false, 0.0f, 0.3f, true);
		rb->velocity.y = 30.0f;
	}

	void PlayerJumpState::OnExit(IStateMachine* machine)
	{
		rb->use_gravity = true;
		rb->velocity.y = -10.0f;
	}

	void PlayerJumpState::Update(IStateMachine* machine, float dt)
	{
		auto pl_machine = static_cast<PlayerStateMachine*>(machine);
#ifdef uuuG // 飛行モード(デバッグ用)
		if (pl_machine->fly)
		{
			if (Input::GetPadButtonRepeat(0, PadButton::Button4))
				rb->velocity.y = 10.0f;
			else
				rb->velocity.y = 0.0f;


			rb->use_gravity = false;
			animator->PlayIfNoSame("fall_loop", true, 0.0f, 0.2f, false);
		}
		else if (!rb->use_gravity) {
			rb->use_gravity = true;
		}

#endif
		Vector3 input = static_cast<PlayerStateMachine*>(machine)->move_input;
		Transform* pl_trns = player->transform.get();
		Vector3 forward = pl_trns->AxisZ();
		
		float dot = forward.dot(input);


		dot = physx::PxClamp(dot, -1.0f, 1.0f);
		float angle = RAD2DEG(acosf(dot));
		float rot_angle = 720 / angle;
		if (dot < 0.0f)
			rot_angle *= 5;
		pl_trns->SetAxisZ(Lerp(pl_trns->AxisZ(), input, rot_angle * Time::DeltaTime()));

		input = input * -20;
		if (pl_machine->fly)
			input *= 2;
		input.y = rb->velocity.y;	// 重力成分を維持
		rb->velocity = input;
	}

	void PlayerJumpState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{

	}


}