#include "precompile.h"
#include "EnemyDamageState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade
{

	EnemyDamageState::EnemyDamageState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();

		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= DAMAGE_DURATION;
			};
		RegisterChangeRequest("idle", default_exit);
	}

	void EnemyDamageState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("enemy_damage");
	}

	void EnemyDamageState::OnExit(IStateMachine* machine)
	{}

	void EnemyDamageState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
	}

	bool EnemyDamageState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "damage" || state_name == "knock_back")
			return true;
		return false;
	}

}