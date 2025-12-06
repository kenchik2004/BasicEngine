#include "precompile.h"
#include "PlayerStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"

#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerMoveState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerIdleState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerJumpState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerFallState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerCombatComboState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerCombatComboState2.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerCombatComboState3.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerClimbingState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerJumpAttackState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerJumpAttackLandState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerDodgeState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerDamageState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerSmashChargeState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerSmashMainState.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerAttack1State.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerAttack2State.h"
#include "Game/Utilitys/NeonFade/States/PlayerStates/PlayerAttack3State.h"

namespace NeonFade
{

	PlayerStateMachine::PlayerStateMachine(Player* owner_) :
		IStateMachine(static_cast<GameObject*>(owner_))
	{
		attack_index_prev = 0;
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
		jump_state->RegisterChangeRequest("attack", jump_to_jump_attack, 0);
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
		fall_state->RegisterChangeRequest("attack", fall_to_jump_attack, 0);
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

#if 0
		if constexpr (false) {
			auto attack1_state = make_safe_unique<PlayerAttack1State>(player);

			//AddState("attack", std::move(attack1_state));
			attack_states_vec[0] = std::move(attack1_state);
		}
		else if constexpr (false) {
			auto attack2_state = make_safe_unique<PlayerAttack2State>(player);

			//AddState("attack", std::move(attack2_state));
			attack_states_vec[1] = std::move(attack2_state);
		}
		else if constexpr (false) {
			auto attack3_state = make_safe_unique<PlayerAttack3State>(player);
			//AddState("attack", std::move(attack3_state));
			attack_states_vec[2] = std::move(attack3_state);
		}
		else if constexpr (false) {
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
			auto attack2_state = make_safe_unique<PlayerCombatComboState2>(player);
			std::function<bool()> attack2_to_jump = [this]() {
				return is_jumping;
				};
			std::function<bool()> attack2_to_dodge = [this]() {
				return is_dodging;
				};
			std::function<bool()> attack2_to_damage = [this]() {
				return is_damaged;
				};
			attack2_state->RegisterChangeRequest("dodge", attack2_to_dodge, 0);
			attack2_state->RegisterChangeRequest("jump", attack2_to_jump, 0);
			attack2_state->RegisterChangeRequest("damage", attack2_to_damage, 1);
			AddState("attack2", std::move(attack2_state));
			auto attack3_state = make_safe_unique<PlayerCombatComboState3>(player);
			std::function<bool()> attack3_to_jump = [this]() {
				return is_jumping;
				};
			std::function<bool()> attack3_to_dodge = [this]() {
				return is_dodging;
				};
			std::function<bool()> attack3_to_damage = [this]() {
				return is_damaged;
				};
			attack3_state->RegisterChangeRequest("dodge", attack3_to_dodge, 0);
			attack3_state->RegisterChangeRequest("jump", attack3_to_jump, 0);
			attack3_state->RegisterChangeRequest("damage", attack3_to_damage, 1);
			AddState("attack3", std::move(attack3_state));
		}

		if constexpr (false) {

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
		}
		else {
			auto smash_charge_state = make_safe_unique<PlayerSmashChargeState>(player);
			AddState("jump_attack", std::move(smash_charge_state));
			auto smash_main_state = make_safe_unique<PlayerSmashMainState>(player);
			AddState("smash_attack", std::move(smash_main_state));
		}

#else

		auto attack1_state = make_safe_unique<PlayerAttack1State>(player);
		attack1_state->SetName("attack1");
		//AddState("attack", std::move(attack1_state));
		attack_states_vec[4] = std::move(attack1_state);

		auto attack2_state = make_safe_unique<PlayerAttack2State>(player);
		attack2_state->SetName("attack2");
		//AddState("attack", std::move(attack2_state));
		attack_states_vec[2] = std::move(attack2_state);
		auto attack3_state = make_safe_unique<PlayerAttack3State>(player);
		attack3_state->SetName("attack3");
		//AddState("attack", std::move(attack3_state));
		attack_states_vec[3] = std::move(attack3_state);


		auto smash_charge_state = make_safe_unique<PlayerSmashChargeState>(player);
		smash_charge_state->SetName("smash_charge");
		//AddState("jump_attack", std::move(smash_charge_state));
		attack_states_vec[1] = std::move(smash_charge_state);
		auto smash_main_state = make_safe_unique<PlayerSmashMainState>(player);
		AddState("smash_attack", std::move(smash_main_state));
		//
		{
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
			attack_state->SetName("default_attack");
			attack_states_vec[0] = std::move(attack_state);
			auto attack2_state = make_safe_unique<PlayerCombatComboState2>(player);
			std::function<bool()> attack2_to_jump = [this]() {
				return is_jumping;
				};
			std::function<bool()> attack2_to_dodge = [this]() {
				return is_dodging;
				};
			std::function<bool()> attack2_to_damage = [this]() {
				return is_damaged;
				};
			attack2_state->RegisterChangeRequest("dodge", attack2_to_dodge, 0);
			attack2_state->RegisterChangeRequest("jump", attack2_to_jump, 0);
			attack2_state->RegisterChangeRequest("damage", attack2_to_damage, 1);
			AddState("attack2", std::move(attack2_state));
			auto attack3_state = make_safe_unique<PlayerCombatComboState3>(player);
			std::function<bool()> attack3_to_jump = [this]() {
				return is_jumping;
				};
			std::function<bool()> attack3_to_dodge = [this]() {
				return is_dodging;
				};
			std::function<bool()> attack3_to_damage = [this]() {
				return is_damaged;
				};
			attack3_state->RegisterChangeRequest("dodge", attack3_to_dodge, 0);
			attack3_state->RegisterChangeRequest("jump", attack3_to_jump, 0);
			attack3_state->RegisterChangeRequest("damage", attack3_to_damage, 1);
			AddState("attack3", std::move(attack3_state));
		}



#endif



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
	void PlayerStateMachine::Update(float dt)
	{
		if (GetCurrentStateName() != "attack") {
			u32 attack_index = 0;
			// ボタン入力に応じてattack_indexを設定

			if (Input::GetPadButtonRepeat(0, PadButton::Button1))
			{

				attack_index = 1;
			}

			if (Input::GetPadButtonRepeat(0, PadButton::Button2))
			{
				attack_index = 2;
			}
			if (Input::GetPadButtonRepeat(0, PadButton::Button3))
			{
				attack_index = 3;
			}
			if (Input::GetPadButtonRepeat(0, PadButton::Button4))
			{
				attack_index = 4;
			}
			if (GetCurrentStateName() == "jump" || GetCurrentStateName() == "fall") {
				attack_index = 1; //空中攻撃はsmash_chargeに固定
			}
			printfDx("attack_index:%d\n", attack_index);
			int i = 0;
			for (auto& state : attack_states_vec) {
				if (state) {
					printfDx("index:%d,name:%s\n", i, state->GetName().c_str());
				}
				else
				{
					printfDx("index:%d,name:null\n", i);
				}
				i++;
			}
			//前フレームと違う攻撃が選択されたら状態を入れ替え

			//既にある攻撃状態を元のスロットに戻す
			if (states["attack"] && attack_index != attack_index_prev)
				attack_states_vec[attack_index_prev] = std::move(states["attack"]);
			//新しい攻撃状態をセット
			if (attack_index != attack_index_prev)
				states["attack"] = std::move(attack_states_vec[attack_index]);

			//上記の操作を経た後に、もし攻撃状態が空ならデフォルトの攻撃状態をセット
			if (!states["attack"])
				states["attack"] = std::move(attack_states_vec[0]);

			//現在フレームの攻撃インデックスを保存
			attack_index_prev = attack_index;
		}

		__super::Update(dt);
	}
}
