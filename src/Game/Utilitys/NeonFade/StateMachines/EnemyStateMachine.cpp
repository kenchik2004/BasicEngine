#include "precompile.h"
#include "EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"

#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyIdleState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDamageState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDownState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDieState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyAttackChargeState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyAttackMainState.h"

namespace NeonFade {
	EnemyStateMachine::EnemyStateMachine(Enemy* owner_)
		:IStateMachine(static_cast<GameObject*>(owner_))
	{
		enemy = owner_;
		auto idle_state = make_safe_unique<EnemyIdleState>(enemy);
		std::function<bool()> idle_to_damage = [this]() {
			return is_damaged;
			};
		std::function<bool()> idle_to_down = [this]() {
			return knock_back;
			};
		std::function<bool()> move_to_die = [this]() {
			return is_dead;
			};

		std::function<bool()> idle_to_move = [this]() {
			return move_vec.magnitudeSquared() > 1e-6f * 1e-6f;
			};
		std::function<bool()>  idle_to_attack_charge = [this]() {
			return is_attacking;
			};

		idle_state->RegisterChangeRequest("die", move_to_die, 0);
		idle_state->RegisterChangeRequest("damage", idle_to_damage, 0);
		idle_state->RegisterChangeRequest("down", idle_to_down, 0);
		idle_state->RegisterChangeRequest("attack_charge", idle_to_attack_charge, 1);
		//idle_state->RegisterChangeRequest("move", idle_to_move, 1);

		AddState("idle", std::move(idle_state));

		auto damage_state = make_safe_unique<EnemyDamageState>(enemy);

		std::function<bool()> damage_to_die = [this]() {
			return is_dead;
			};
		damage_state->RegisterChangeRequest("die", damage_to_die, 0);
		AddState("damage", std::move(damage_state));

		auto down_state = make_safe_unique<EnemyDownState>(enemy);
		std::function<bool()> down_to_die = [this, state = down_state.get()]() {
			return is_dead && state->exit_timer >= state->EXIT_TIME;
			};
		down_state->RegisterChangeRequest("die", down_to_die, 0);
		AddState("down", std::move(down_state));

		auto die_state = make_safe_unique<EnemyDieState>(enemy);
		AddState("die", std::move(die_state));

		auto attack_charge_state = make_safe_unique<EnemyAttackChargeState>(enemy);
		std::function<bool()> charge_to_die = [this]() {
			return is_dead;
			};
		attack_charge_state->RegisterChangeRequest("die", charge_to_die, 0);
		AddState("attack_charge", std::move(attack_charge_state));
		auto attack_main_state = make_safe_unique<EnemyAttackMainState>(enemy);
		std::function<bool()> attack_to_idle = [state = attack_main_state.get()]() {
			return state->attack_timer >= state->ATTACK_TIME;
			};
		std::function<bool()> attack_to_die = [this]() {
			return is_dead;
			};
		attack_main_state->RegisterChangeRequest("die", attack_to_die, 0);
		attack_main_state->RegisterChangeRequest("idle", attack_to_idle, 0);
		AddState("attack_main", std::move(attack_main_state));


		ChangeState("idle");

	}
	void EnemyStateMachine::DebugDraw()
	{
		printfDx("%s : %s\n", enemy->name.c_str(), GetCurrentStateName().c_str());
	}
	void EnemyStateMachine::OnTriggerEnter(const HitInfo& hit_info)
	{
		if(current_state)
			current_state->OnTriggerEnter(this, hit_info);
	}
	EnemyStateMachine::~EnemyStateMachine()
	{
	}
}
