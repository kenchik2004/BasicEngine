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
		RegisterChangeRequest("idle", default_exit, 1);
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
		if (state_name == "damage" || state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}

	EnemyCrowlingDamageState::EnemyCrowlingDamageState(Enemy* owner_enemy_)
		:EnemyDamageState(owner_enemy_)
	{

		rb = owner_enemy->rb.lock().get();
		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= DAMAGE_DURATION;

			};
		RegisterChangeRequest("crowling", default_exit, 0);
	}

	void EnemyCrowlingDamageState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		if (animator)
			animator->Play("enemy_damage_crowling");
		rb->velocity.y = 8.0f;

	}

	bool EnemyCrowlingDamageState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "damage_crowling" || state_name == "knock_front" || state_name == "knock_back" || state_name == "die")
			return true;
		return false;
	}

}