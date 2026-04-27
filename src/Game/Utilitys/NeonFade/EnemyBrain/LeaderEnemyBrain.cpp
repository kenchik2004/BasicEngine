//---------------------------------------------------------------------------
//! @file   LeaderEnemyBrain.cpp
//! @brief  LeaderEnemyBrainの実装。リーダー敵のAI思考ロジックを実装する
//---------------------------------------------------------------------------
#include "LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyTeam.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyLeaderAttackState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyLeaderStepBackState.h"

namespace NeonFade
{
	LeaderEnemyBrain::LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		my_team = team;
		machine = state_machine_;
		player = player_;
		hp = MAX_HP;
		{
			auto instruct_state = make_safe_unique<EnemyLeaderAttackState>(machine->enemy, player);
			std::function<bool()> instruct_to_die = [this]() {
				return machine->is_dead;
				};
			std::function<bool()> instruct_to_damage = [this]() {
				return static_cast<bool>(machine->is_damaged);
				};
			std::function<bool()> instruct_to_down = [this]() {
				return machine->knock_back;
				};
			std::function<bool()> instruct_to_stepback = [this]() {
				return static_cast<bool>(machine->is_step_back);
				};
			instruct_state->RegisterChangeRequest("die", instruct_to_die, 0);
			instruct_state->RegisterChangeRequest("damage", instruct_to_damage, 0);
			instruct_state->RegisterChangeRequest("down", instruct_to_down, 0);
			instruct_state->RegisterChangeRequest("step_back", instruct_to_stepback, 0);

			//攻撃とは言っているが、実際には部下に指示を出す状態(部下はリーダーの"attack"状態を見て行動するため)
			machine->AddState("attack_instruct", std::move(instruct_state));
		}
		{
			auto step_back_state = make_safe_unique<EnemyLeaderStepBackState>(machine->enemy, player);
			std::function<bool()> stepback_to_damage = [this]() {
				return static_cast<bool>(machine->is_damaged);
				};
			std::function<bool()> stepback_to_die = [this]() {
				return machine->is_dead;
				};
			std::function<bool()> stepback_to_down = [this]() {
				return machine->knock_back;
				};
			step_back_state->RegisterChangeRequest("die", stepback_to_die, 0);
			step_back_state->RegisterChangeRequest("damage", stepback_to_damage, 0);
			step_back_state->RegisterChangeRequest("down", stepback_to_down, 0);
			machine->AddState("step_back", std::move(step_back_state));

		}

		{
			update_selector = [this]()->std::string {
				bool dmg_bool = static_cast<bool>(is_damaged);
				bool kb_bool = static_cast<bool>(knock_back);
				if (hp == 0 || machine->enemy->transform->position.y < -50)
					return "die";
				if (kb_bool)
					return "knock_back";
				if (dmg_bool)
					return "damage";

				u32 weakness = 0;	//弱虫度(体力が少ない、プレイヤーが近付いた等で変動)
				u32 aggression = 0; //攻撃的度(プレイヤーが攻撃してこない等で変動)

				//部下は近付いて攻撃するが、リーダーは少し離れて指揮を取るイメージ
				//近いほど距離を取ろうとし、遠いほど扇動的になる
				if (player) {
					Vector3 to_player = player->transform->position - GetOwnerBody()->transform->position;
					float distance = to_player.magnitude();
					static constexpr float max_pl_dist = 100.0f;
					distance = min(distance, max_pl_dist);

					//プレイヤーが近いほど弱虫度アップ
					float dist_factor = 1.0f - distance / max_pl_dist;
					weakness += static_cast<u32>(dist_factor * 20.0f);
					//プレイヤーが遠いほど攻撃的度アップ
					float aggro_factor = (distance / max_pl_dist);
					aggression += static_cast<u32>(aggro_factor * 10.0f);

					//体力が少ないほど弱虫度アップ
					float hp_factor = 1.0f - (static_cast<float>(hp) / static_cast<float>(MAX_HP));
					weakness += static_cast<u32>(hp_factor * 30.0f);
					if (my_team) {
						//部下が多いほど攻撃的度アップ
						u32 member_count = static_cast<u32>(my_team->GetMemberNum());
						aggression += member_count * 5;
						//部下が少ないほど弱虫度アップ
						weakness += member_count > 5 ? (5 - member_count) * 3 : 0;
					}

					//高い方の行動を選択
					std::pair<std::string, u32> scores[] = {
						{"step_back", weakness},
						{"instruct",aggression},
					};
					auto max_elem = std::max_element(std::begin(scores), std::end(scores),
						[](const auto& a, const auto& b) {
							return a.second < b.second;
						});
					return max_elem->first;



				}

				return "default";
				};


			std::function<void()> default_think = [this]() {
				//何もしない
				};
			std::function<void()> damage_think = [this]() {
				machine->is_damaged = true;
				};
			std::function<void()> knock_back_think = [this]() {
				machine->knock_back = true;
				};
			std::function<void()> die_think = [this]() {
				machine->is_dead = true;
				};
			std::function<void()> instruct_think = [this]() {
				machine->is_instructing = true;
				};
			std::function<void()> stepback_think = [this]() {
				machine->is_step_back = true;
				};


			thinkings = {
				{"default", default_think},
				{"damage", damage_think},
				{"knock_back", knock_back_think},
				{"die", die_think},
				{"instruct", instruct_think},
				{"step_back", stepback_think},
			};
		}
	}
	LeaderEnemyBrain::~LeaderEnemyBrain()
	{
		RemoveFromTeam();
	}
	void LeaderEnemyBrain::Think()
	{
		i_frame_timer -= Time::DeltaTime();
		thinkings[update_selector()]();
	}
	void LeaderEnemyBrain::Damage(u32 damage, bool ignore_i_frame)
	{
		if (!ignore_i_frame && i_frame_timer > 0.0f)
			return;
		if (damage <= hp)
			hp -= damage;
		else
			hp = 0;
		i_frame_timer = I_FRAME;
		is_damaged = true;
	}
	void LeaderEnemyBrain::Die() {
		hp = 0;
	}
	void LeaderEnemyBrain::DebugDraw()
	{
	}
	void LeaderEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		knock_back = true;
		machine->move_vec = knock_back_vec;
	}
	void LeaderEnemyBrain::RemoveFromTeam()
	{
		if (my_team) {
			my_team->SetLeader(nullptr);
			my_team = nullptr;
		}
	}
}