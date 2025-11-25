#include "PlayerClimbingState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
namespace NeonFade
{
	PlayerClimbingState::PlayerClimbingState(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		owner_player = owner_;
		rb = owner_player->rb.lock().get();
		animator = owner_player->animator.lock().get();
		std::function<bool()> exit_request = [this]() {
			return climb_time <= 0.0f;
			};

		RegisterChangeRequest("idle", exit_request, 0);
	}
	void PlayerClimbingState::OnEnter(IStateMachine* machine)
	{
		climb_time = 1.5f;
		rb->velocity = Vector3(0, 0, 0);
		rb->use_gravity = false;
		rb->is_kinematic = true;
		animator->Play("climb", false, 0.0f, 0.2f, true);
		auto pl_machine = static_cast<PlayerStateMachine*>(machine);
		climb_start_pos = pl_machine->climb_start_pos;
		climb_end_pos = pl_machine->climb_end_pos;
	}
	void PlayerClimbingState::OnExit(IStateMachine* machine)
	{
		rb->use_gravity = true;
		rb->is_kinematic = false;
	}
	void PlayerClimbingState::Update(IStateMachine* machine, float dt)
	{
		climb_time -= Time::RealDeltaTime();
		if (climb_time < 1.0f) {
			owner_player->transform->position = Lerp(climb_start_pos, climb_end_pos, (1.0f - climb_time));
		}
	}
}
