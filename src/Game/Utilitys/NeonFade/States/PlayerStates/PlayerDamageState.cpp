#include "PlayerDamageState.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade
{
	PlayerDamageState::PlayerDamageState(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		animator = player->animator.lock().get();
		
		std::function default_change = [this]() {
			return exit_timer >= EXIT_TIME;
			};
		RegisterChangeRequest("idle", default_change, 1);
	}
	void PlayerDamageState::OnEnter(IStateMachine* machine)
	{
		animator->anim_speed = 2.0f;
		animator->PlayIfNoSame("player_damage");
		exit_timer = 0;
		//rb->velocity = { 0, 0, 0 };
	}
	void PlayerDamageState::OnExit(IStateMachine* machine)
	{
		//animator->Stop();
		animator->anim_speed = 1.0f;
	}
	void PlayerDamageState::Update(IStateMachine* machine, float dt)
	{
		exit_timer += dt;
	}
}