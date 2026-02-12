#include "TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/PlayerController.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyEscapeState.h"
#include "Game/Utilitys/NeonFade/EnemyTeam.h"

namespace NeonFade {

	TeamMemberEnemyBrain::TeamMemberEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		machine = state_machine_;
		my_team = team;
		hp = MAX_HP;

		{
			SafeWeakPtr<Object> team_leader = nullptr;
			if (team && team->GetLeader()) {

				team_leader = SafeSharedPtr(team->GetLeader()->GetOwnerBody()->shared_from_this());
			}
			auto escape_state = make_safe_unique<EnemyEscapeState>(machine->enemy, team_leader);
			std::function<bool()> escape_to_die = [state_machine_]() {
				return state_machine_->is_dead;
				};
			std::function<bool()> escape_to_damage = [state_machine_]() {
				return static_cast<bool>(state_machine_->is_damaged);
				};
			std::function<bool()> escape_to_down = [state_machine_]() {
				return static_cast<bool>(state_machine_->knock_back);
				};

			escape_state->RegisterChangeRequest("die", escape_to_die, 0);
			escape_state->RegisterChangeRequest("damage", escape_to_damage, 0);
			escape_state->RegisterChangeRequest("down", escape_to_down, 0);

			state_machine_->AddState("escape", std::move(escape_state));
		}
		update_selector = [this]()->std::string {
			bool dmg_bool = is_damaged;
			bool kb_bool = knock_back;
			if (kb_bool)
				return "knock_back";
			if (dmg_bool)
				return "damage";
			if (hp == 0)
				return "die";
			{
				//ここで各行動に点数をつけて、一番高い行動を返すようにする
				u32 weakness = 0;	//弱虫度(リーダーが近くにいない、体力が少ない等で変動)
				u32 aggression = 0; //攻撃的度(プレイヤーが攻撃してこない、リーダーが攻撃している等で変動)
				u32 confusion = 0;	//混乱度(チームに所属していない、リーダーが倒された等で変動)
				if (my_team) {
					if (auto leader_brain = my_team->GetLeader()) {
						//リーダーがいる場合
						std::string leader_state = leader_brain->GetMachine()->GetCurrentStateName();
						if (leader_state.find("attack") != leader_state.npos)
							aggression += 5; //リーダーが攻撃していると攻撃的度アップ

						if (auto leader = leader_brain->GetMachine()->enemy) {
							Vector3 to_leader = leader->transform->position - machine->enemy->transform->position;
							float dist = std::clamp(to_leader.magnitude(), 1.0f, 30.0f);
							weakness += std::lerp(0, 10, dist * (1 / 30.0f)); //リーダーから遠いほど弱虫度アップ(最大10)
						}
					}
					else {
						//リーダーが倒されている場合、混乱度アップ
						confusion += 15;

					}
				}
				else {
					confusion += 15;	//チームに所属していないと混乱度アップ

				}
				weakness += (MAX_HP - hp) / 20;	//体力が減るほど弱虫度アップ
				confusion += (MAX_HP - hp) / 30;	//体力が減るほど混乱度アップ
				aggression += hp / 20;			//体力が多いほど攻撃的度アップ
				if (auto player_sp = player.lock()) {

					Vector3 to_player = player_sp->transform->position - machine->enemy->transform->position;
					float dist = std::clamp(to_player.magnitude(), 1.0f, 30.0f);

					weakness += std::lerp(10, 0, dist * (1 / 30.0f)); //遠ければ弱虫度アップ
					const std::string& player_state = player_sp->pl_controller->GetStateMachine()->GetCurrentStateName();
					if (player_state.find("attack") == player_state.npos)
						aggression += 5; //プレイヤーが攻撃してこないと攻撃的度アップ
				}
				std::pair<std::string, u32> scores[] = {
					{"escape", weakness},
					{"welter",confusion},
					{"attack",aggression},
				};
				auto max_elem = std::max_element(std::begin(scores), std::end(scores),
					[](const auto& a, const auto& b) {
						return a.second < b.second;
					});
				if (max_elem->second <= 5)
					return "default";
				return max_elem->first;


			}
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
		std::function<void()> attack_think = [this]() {
			machine->is_attacking = true;
			};
		std::function<void()> escape_think = [this]() {
			machine->is_escaping = true;
			};
		std::function<void()> welter_think = [this]() {
			machine->is_confused = true;
			};

		thinkings = {
			{"default", default_think},
			{"damage", damage_think},
			{"knock_back", knock_back_think},
			{"die", die_think},
			{"attack", attack_think},
			{"escape", escape_think},
			{"welter", welter_think},
		};

	}

	TeamMemberEnemyBrain::~TeamMemberEnemyBrain()
	{
		if (my_team) {
			my_team->SubMember(this);
		}
	}

	void TeamMemberEnemyBrain::Think()
	{

		thinkings[update_selector()]();
		if (i_frame_timer > 0.0f) {
			i_frame_timer -= Time::DeltaTime();
		}
	}

	void TeamMemberEnemyBrain::AddLeader(LeaderEnemyBrain* new_leader)
	{
	}

	void TeamMemberEnemyBrain::Damage(u32 damage, bool ignore_i_frame)
	{
		if (!ignore_i_frame && i_frame_timer > 0.0f)
			return;
		is_damaged = true;
		if (damage <= hp)
			hp -= damage;
		else
			hp = 0;
		i_frame_timer = I_FRAME;
	}

	void TeamMemberEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		knock_back = true;
		machine->move_vec = knock_back_vec;
	}
	void TeamMemberEnemyBrain::DebugDraw()
	{
	}
	void TeamMemberEnemyBrain::RemoveFromTeam()
	{
		if (my_team) {
			my_team->SubMember(this);
			my_team = nullptr;
		}
	}
	void TeamMemberEnemyBrain::Die() {
		hp = 0;
	}

}