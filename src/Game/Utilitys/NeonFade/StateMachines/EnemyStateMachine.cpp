#include "precompile.h"
#include "EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"

#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyIdleState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDamageState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDownState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDieState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyAttackChargeState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyAttackMainState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyMoveState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyAttackReadyState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyWelterState.h"

namespace NeonFade {
	EnemyStateMachine::EnemyStateMachine(Enemy* owner_)
		:IStateMachine(static_cast<GameObject*>(owner_))
	{
		enemy = owner_;
		auto idle_state = make_safe_unique<EnemyIdleState>(enemy);
		std::function<bool()> idle_to_damage = [this]() {
			return static_cast<bool>(is_damaged);
			};
		std::function<bool()> idle_to_down = [this]() {
			return static_cast<bool>(knock_back);
			};
		std::function<bool()> idle_to_die = [this]() {
			return is_dead;
			};
		std::function<bool()> idle_to_welter = [this]() {
			return is_confused;
			};
		std::function<bool()> idle_to_escape = [this]() {
			return is_escaping;
			};

		std::function<bool()> idle_to_move = [this]() {
			return move_vec.magnitudeSquared() > 1e-6f * 1e-6f;
			};
		std::function<bool()>  idle_to_attack_ready = [this]() {
			return static_cast<bool>(is_attacking);
			};

		idle_state->RegisterChangeRequest("down", idle_to_down, 0);
		idle_state->RegisterChangeRequest("die", idle_to_die, 0);
		idle_state->RegisterChangeRequest("damage", idle_to_damage, 0);
		idle_state->RegisterChangeRequest("attack_ready", idle_to_attack_ready, 1);
		idle_state->RegisterChangeRequest("welter", idle_to_welter, 1);
		idle_state->RegisterChangeRequest("escape", idle_to_escape, 1);
		idle_state->RegisterChangeRequest("move", idle_to_move, 1);

		AddState("idle", std::move(idle_state));


		auto move_state = make_safe_unique<EnemyMoveState>(enemy);
		std::function<bool()> move_to_idle = [this]() {
			return move_vec.magnitudeSquared() < 1e-6f * 1e-6f;
			};
		std::function<bool()> move_to_damage = [this]() {
			return static_cast<bool>(is_damaged);
			};
		std::function<bool()> move_to_down = [this]() {
			return static_cast<bool>(knock_back);
			};
		std::function<bool()> move_to_attack_ready = [this]() {
			return static_cast<bool>(is_attacking);
			};
		std::function<bool() > move_to_die = [this]() {
			return is_dead;
			};

		std::function<bool()> move_to_welter = [this]() {
			return is_confused;
			};
		std::function<bool()> move_to_escape = [this]() {
			return is_escaping;
			};
		move_state->RegisterChangeRequest("down", move_to_down, 0);
		move_state->RegisterChangeRequest("damage", move_to_damage, 0);
		move_state->RegisterChangeRequest("die", move_to_die, 0);
		move_state->RegisterChangeRequest("idle", move_to_idle, 1);
		move_state->RegisterChangeRequest("welter", move_to_welter, 1);
		move_state->RegisterChangeRequest("escape", move_to_escape, 1);
		move_state->RegisterChangeRequest("attack_ready", move_to_attack_ready, 0);
		AddState("move", std::move(move_state));


		auto damage_state = make_safe_unique<EnemyDamageState>(enemy);

		std::function<bool()> damage_to_die = [this]() {
			return is_dead;
			};
		std::function<bool()> re_damage = [this]() {
			return static_cast<bool>(is_damaged);
			};
		std::function<bool()> damage_to_knockback = [this]() {
			return static_cast<bool>(knock_back);
			};
		damage_state->RegisterChangeRequest("die", damage_to_die, 0);
		damage_state->RegisterChangeRequest("down", damage_to_knockback, 0);
		damage_state->RegisterChangeRequest("damage", re_damage, 1);
		AddState("damage", std::move(damage_state));

		auto down_state = make_safe_unique<EnemyDownState>(enemy);
		std::function<bool()> down_to_die = [this, state = down_state.get()]() {
			return is_dead && state->exit_timer >= state->EXIT_TIME;
			};
		std::function<bool()> re_down = [this]() {
			return static_cast<bool>(knock_back);
			};
		down_state->RegisterChangeRequest("die", down_to_die, 0);
		down_state->RegisterChangeRequest("down", re_down, 1);
		AddState("down", std::move(down_state));

		auto die_state = make_safe_unique<EnemyDieState>(enemy);
		AddState("die", std::move(die_state));
		auto attack_ready_state = make_safe_unique<EnemyAttackReadyState>(enemy);
		std::function<bool()> attack_to_knockback = [this]() {
			return static_cast<bool>(knock_back);
			};
		std::function<bool()> attack_ready_to_damage = [this]() {
			return static_cast<bool>(is_damaged);
			};
		std::function<bool()> attack_ready_to_die = [this]() {
			return is_dead;
			};
		attack_ready_state->RegisterChangeRequest("down", attack_to_knockback, 0);
		attack_ready_state->RegisterChangeRequest("die", attack_ready_to_die, 0);
		attack_ready_state->RegisterChangeRequest("damage", attack_ready_to_damage, 0);

		AddState("attack_ready", std::move(attack_ready_state));
		auto attack_charge_state = make_safe_unique<EnemyAttackChargeState>(enemy);
		std::function<bool()> charge_to_damage = [this]() {
			return static_cast<bool>(is_damaged);
			};
		std::function<bool()> charge_to_knockback = [this]() {
			return static_cast<bool>(knock_back);
			};
		std::function<bool()> charge_to_die = [this]() {
			return is_dead;
			};
		attack_charge_state->RegisterChangeRequest("die", charge_to_die, 0);
		attack_charge_state->RegisterChangeRequest("damage", charge_to_damage, 0);
		attack_charge_state->RegisterChangeRequest("down", charge_to_knockback, 0);
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
		
		auto welter_state = make_safe_unique<EnemyWelterState>(enemy);
		std::function<bool()> welter_to_damage = [this]() {
			return static_cast<bool>(is_damaged);
			};
		std::function<bool()> welter_to_die = [this]() {
			return is_dead;
			};
		std::function<bool()> welter_to_down = [this]() {
			return static_cast<bool>(knock_back);
			};
		welter_state->RegisterChangeRequest("die", welter_to_die, 0);
		welter_state->RegisterChangeRequest("damage", welter_to_damage, 0);
		welter_state->RegisterChangeRequest("down", welter_to_down, 0);
		AddState("welter", std::move(welter_state));


		ChangeState("idle");

	}
	void EnemyStateMachine::DebugDraw()
	{
		auto& cur_state = current_state->GetName();
		printfDx("%s : %s\n", enemy->name.c_str(), cur_state.c_str());
	}
	void EnemyStateMachine::OnTriggerEnter(const HitInfo& hit_info)
	{
		if (current_state)
			current_state->OnTriggerEnter(this, hit_info);
	}
	EnemyStateMachine::~EnemyStateMachine()
	{
	}
}
