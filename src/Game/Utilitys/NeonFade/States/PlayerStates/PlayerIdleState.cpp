#include "PlayerIdleState.h"

#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"

namespace NeonFade
{
	PlayerIdleState::PlayerIdleState(Player* owner_) :
		IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		rb = player->rb.lock().get();
		animator = player->animator.lock().get();
	}
	void PlayerIdleState::OnEnter(IStateMachine* machine)
	{
		animator->Play("idle", true, 0.0f, 0.1f, false);
	}
	void PlayerIdleState::OnExit(IStateMachine* machine)
	{
	}
	void PlayerIdleState::Update(IStateMachine* machine, float dt)
	{
		Vector3 force = rb->velocity * -0.5f;
		force.y = 0;
		rb->AddForce(force, ForceMode::Impulse); // 慣性を打ち消す
		bool input_exist = false;
		//-------------------------------------------------
		Vector2 pad_left = Input::GetPadLeftStick(0);

		if (pad_left.magnitudeSquared() >= FLT_EPSILON) {
			input_exist = true;
		}
		if (Input::GetKey(KeyCode::W) || Input::GetKey(KeyCode::S) || Input::GetKey(KeyCode::A) || Input::GetKey(KeyCode::D))
			input_exist = true;
		auto current_velocity = rb->velocity;
		//rb->velocity = { 0,current_velocity.y,0 };
	}
	void PlayerIdleState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{
	}
}