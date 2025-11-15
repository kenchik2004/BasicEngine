#include "precompile.h"
#include "PlayerFallState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
#include "System/Components/Animator.h"
#include "System/Components/Collider.h"

namespace NeonFade {
	PlayerFallState::PlayerFallState(Player* player_) :
		IState(static_cast<GameObject*>(player_))
	{
		player = player_;
		animator = player->animator.lock().get();

	}
	void PlayerFallState::OnEnter(IStateMachine* machine)
	{
		animator->Play("fall_loop", true, 0.0f, 0.7f, true);
	}
	void PlayerFallState::OnExit(IStateMachine* machine)
	{
	}
	void PlayerFallState::Update(IStateMachine* machine, float dt)
	{
	}
	void PlayerFallState::OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info)
	{
	}
}