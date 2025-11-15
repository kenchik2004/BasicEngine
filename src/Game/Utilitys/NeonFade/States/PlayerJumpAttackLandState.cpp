#include "precompile.h"
#include "PlayerJumpAttackLandState.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade {

	PlayerJumpAttackLandState::PlayerJumpAttackLandState(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		owner_player = owner_;
		rb = owner_player->rb.lock().get();
		animator = owner_player->animator.lock().get();
		std::function animation_cut_callback = [this]() {
			animator->Pause();
			};
		animator->SetAnimationCallBack("back_flip", animation_cut_callback, 40, "back_flip_end");

	}
	void PlayerJumpAttackLandState::OnEnter(IStateMachine* machine)
	{
		animator->Play("back_flip", false, 0.0f, 0.1f, true);
		Vector3 player_back_up_vec = owner_player->transform->AxisZ() + owner_player->transform->AxisY();
		player_back_up_vec = player_back_up_vec.getNormalized() * 50;
		rb->velocity = player_back_up_vec;
	}
	void PlayerJumpAttackLandState::OnExit(IStateMachine* machine)
	{
	}
	void PlayerJumpAttackLandState::Update(IStateMachine* machine, float dt)
	{
	}
}
