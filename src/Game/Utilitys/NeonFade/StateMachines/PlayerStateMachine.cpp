#include "precompile.h"
#include "PlayerStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"

#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerMoveState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerIdleState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerJumpState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerFallState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerCombatComboState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerClimbingState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerJumpAttackState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerJumpAttackLandState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerDodgeState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerDamageState.h"

namespace NeonFade
{

	PlayerStateMachine::PlayerStateMachine(Player* owner_) :
		IStateMachine(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		auto move_state = make_safe_unique<PlayerMoveState>(player);
		std::function<bool()> move_to_jump = [this]() {
			return is_jumping;
			};
		std::function<bool()> move_to_idle = [this]() {

			return move_input.magnitudeSquared() < 0.01f;
			};
		std::function <bool()> move_to_fall = [this]() {
			//return  player->rb->velocity.y < -5.0f;
			return is_falling;
			};
		std::function <bool()> move_to_climb = [this]() {
			return can_climb && move_input.magnitudeSquared() > FLT_EPSILON;
			};
		std::function<bool()> move_to_attack = [this]() {
			return is_attacking;
			};
		std::function<bool()> move_to_dodge = [this]() {
			return is_dodging;
			};
		std::function<bool()> move_to_damage = [this]() {
			return is_damaged;
			};
		move_state->RegisterChangeRequest("dodge", move_to_dodge, 0);
		move_state->RegisterChangeRequest("jump", move_to_jump, 0);
		move_state->RegisterChangeRequest("climb", move_to_climb, 0);
		move_state->RegisterChangeRequest("damage", move_to_damage, 1);
		move_state->RegisterChangeRequest("idle", move_to_idle, 1);
		move_state->RegisterChangeRequest("fall", move_to_fall, 1);
		move_state->RegisterChangeRequest("attack", move_to_attack, 1);
		AddState("move", std::move(move_state));

		auto jump_state = make_safe_unique<PlayerJumpState>(player);

		std::function<bool()> jump_to_fall = [this, state = jump_state.get()]() {
			return  is_falling && state->jump_timer >= state->JUMP_TIME;
			};
		std::function<bool()> jump_to_idle = [this, state = jump_state.get()]() {
			return is_landed && state->jump_timer >= state->JUMP_TIME;
			};
		std::function<bool()> jump_to_climb = [this]() {
			return can_climb && move_input.magnitudeSquared() > FLT_EPSILON;
			};
		std::function<bool()> jump_to_jump_attack = [this]() {
			return is_attacking;
			};
		std::function<bool()> jump_to_dodge = [this]() {
			return is_dodging;
			};
		std::function<bool()> jump_to_damage = [this]() {
			return is_damaged;
			};
		jump_state->RegisterChangeRequest("damage", jump_to_damage, 1);
		jump_state->RegisterChangeRequest("fall", jump_to_fall, 1);
		jump_state->RegisterChangeRequest("idle", jump_to_idle, 1);
		jump_state->RegisterChangeRequest("dodge", jump_to_dodge, 0);
		jump_state->RegisterChangeRequest("climb", jump_to_climb, 0);
		jump_state->RegisterChangeRequest("jump_attack", jump_to_jump_attack, 0);
		AddState("jump", std::move(jump_state));


		auto fall_state = make_safe_unique<PlayerFallState>(player);
		std::function<bool()> fall_to_idle = [this]() {
			//return player->rb->velocity.y >= -5.0f;
			return is_landed;
			};
		std::function<bool()> fall_to_climb = [this]() {
			return can_climb && move_input.magnitudeSquared() > FLT_EPSILON;
			};
		std::function<bool()> fall_to_jump_attack = [this]() {return is_attacking; };
		std::function<bool()> fall_to_damage = [this]() {
			return is_damaged;
			};
		fall_state->RegisterChangeRequest("damage", fall_to_damage, 1);
		fall_state->RegisterChangeRequest("idle", fall_to_idle, 1);
		fall_state->RegisterChangeRequest("climb", fall_to_climb, 0);
		fall_state->RegisterChangeRequest("jump_attack", fall_to_jump_attack, 0);
		AddState("fall", std::move(fall_state));

		auto climb_state = make_safe_unique<PlayerClimbingState>(player);

		AddState("climb", std::move(climb_state));

		auto idle_state = make_safe_unique<PlayerIdleState>(player);
		std::function<bool()> idle_to_move = [this]() {
			return move_input.magnitudeSquared() >= 0.01f;
			};
		std::function<bool()> idle_to_jump = [this]() {
			return is_jumping;
			};
		std::function<bool()> idle_to_fall = [this]() {
			//return  player->rb->velocity.y < -5.0f;
			return is_falling;
			};
		std::function<bool()> idle_to_attack = [this]() {
			return is_attacking;
			};
		std::function<bool()> idle_to_dodge = [this]() {
			return is_dodging;
			};
		std::function<bool()> idle_to_damage = [this]() {
			return is_damaged;
			};

		idle_state->RegisterChangeRequest("dodge", idle_to_dodge, 0);
		idle_state->RegisterChangeRequest("jump", idle_to_jump, 0);
		idle_state->RegisterChangeRequest("fall", idle_to_fall, 0);
		idle_state->RegisterChangeRequest("damage", idle_to_damage, 1);
		idle_state->RegisterChangeRequest("move", idle_to_move, 1);
		idle_state->RegisterChangeRequest("attack", idle_to_attack, 1);
		AddState("idle", std::move(idle_state));

		auto attack_state = make_safe_unique<PlayerCombatComboState>(player);
		std::function<bool()> attack_to_jump = [this]() {
			return is_jumping;
			};
		std::function<bool()> attack_to_dodge = [this]() {
			return is_dodging;
			};
		std::function<bool()> attack_to_damage = [this]() {
			return is_damaged;
			};
		attack_state->RegisterChangeRequest("dodge", attack_to_dodge, 0);
		attack_state->RegisterChangeRequest("jump", attack_to_jump, 0);
		attack_state->RegisterChangeRequest("damage", attack_to_damage, 1);
		AddState("attack", std::move(attack_state));

		auto jump_atk_state = make_safe_unique<PlayerJumpAttackState>(player);
		std::function<bool()> jump_atk_to_idle = [this]() {
			return is_landed;
			};
		jump_atk_state->RegisterChangeRequest("idle", jump_atk_to_idle, 0);
		AddState("jump_attack", std::move(jump_atk_state));
		auto jump_atk_land_state = make_safe_unique<PlayerJumpAttackLandState>(player);

		std::function<bool()> atk_land_to_idle = [this]() {
			return is_landed && player->animator->IsPaused();
			};
		std::function<bool()> atk_land_to_fall = [this]() {
			return is_falling && player->animator->IsPaused();
			};
		std::function<bool()> atk_land_to_damage = [this]() {
			return is_damaged;
			};

		jump_atk_land_state->RegisterChangeRequest("idle", atk_land_to_idle, 0);
		jump_atk_land_state->RegisterChangeRequest("fall", atk_land_to_fall, 1);
		jump_atk_land_state->RegisterChangeRequest("damage", atk_land_to_damage, 1);
		AddState("jump_attack_land", std::move(jump_atk_land_state));

		auto dodge_state = make_safe_unique<PlayerDodgeState>(player);
		std::function<bool()> dodge_to_damage = [this, state = dodge_state.get()]() {
			return is_damaged && state->dodge_timer >= state->I_FRAME_TIME;
			};
		AddState("dodge", std::move(dodge_state));

		auto damage_state = make_safe_unique<PlayerDamageState>(player);
		AddState("damage", std::move(damage_state));
	}
	PlayerStateMachine::~PlayerStateMachine()
	{
	}
	void PlayerStateMachine::DebugDraw()
	{
		__super::DebugDraw();
		if (current_state)
			current_state->DebugDraw();
	}
}
