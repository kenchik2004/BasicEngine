#include "PlayerDamageState.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade
{
	PlayerDamageState::PlayerDamageState(Player* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		animator = player->animator.lock().get();
		std::function hit_stop = [this]() {
			animator->anim_speed = 0.001f;
			hit_stop_timer += Time::UnscaledDeltaTime();
			};
		animator->SetAnimationCallBack("player_damage", hit_stop, 3, "hit_stop");
		std::function default_change = [this]() {
			return exit_timer >= EXIT_TIME;
			};
		RegisterChangeRequest("idle", default_change, 1);
	}
	void PlayerDamageState::OnEnter(IStateMachine* machine)
	{
		animator->anim_speed = 2.0f;
		animator->PlayIfNoSame("player_damage");
		hit_stop_timer = 0;
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
		if (hit_stop_timer > 0)
			hit_stop_timer += dt;
		if (hit_stop_timer > 0.1f) {
			animator->anim_speed = 2.0f;
			hit_stop_timer = 0;
		}
	}
}