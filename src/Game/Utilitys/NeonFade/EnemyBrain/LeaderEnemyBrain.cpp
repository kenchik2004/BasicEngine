#include "LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Components/EnemyController.h"

namespace NeonFade {
	LeaderEnemyBrain::LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		team_data.leader = this;
		another_groups.push_back(&team_data);
	}
	LeaderEnemyBrain::~LeaderEnemyBrain()
	{

		auto ite = std::find(another_groups.begin(), another_groups.end(), &team_data);
		if (ite != another_groups.end())
			another_groups.erase(ite);
	}
	void LeaderEnemyBrain::Think()
	{



		bool knock_back_bool = knock_back;
		bool damaged_bool = is_damaged;
		//ここで思考する
		{
			if (!player)
				return;
			randwalk_timer -= Time::UnscaledDeltaTime();
			Vector3 pl_dist = player->transform->position - team_data.team_position;
			pl_dist.y = 0;//高さ方向は無視
			//一旦仮で、プレイヤーから一定距離内に近付き、その中でランダムウォークを行うようにしておく
			if constexpr (true) {
				if (pl_dist.magnitudeSquared() > 30.0f * 30.0f) {
					Vector3 random_walk_vec = next_target - team_data.team_position;
					random_walk_vec.y = 0;		//高さ方向は無視
					if (random_walk_vec.magnitudeSquared() <= 5.0f * 5.0f || random_walk_vec.magnitudeSquared() > 70.0f * 70.0f || randwalk_timer < 0.0f) {
						randwalk_timer = max_randwalk_time;
						next_target = Random::Position(team_data.team_position - Vector3(30, 0, 30), team_data.team_position + Vector3(30, 0, 30));
					}
					random_walk_vec = next_target - team_data.team_position;
					team_data.team_mov_vec = random_walk_vec.getNormalized();

					for (auto& member : team_data.members) {
						member->GetMachine()->is_attacking = false;
					}
					state_machine->is_attacking = false;
				}
				else {
					if (pl_dist.magnitudeSquared() > 5.0f * 5.0f)
						team_data.team_mov_vec = pl_dist.getNormalized() * 7;
					else {
						team_data.team_mov_vec = { 0,0,0 };
						randwalk_timer = max_randwalk_time;
						next_target = Random::Position(team_data.team_position - Vector3(30, 0, 30), team_data.team_position + Vector3(30, 0, 30));
					}

					for (auto& member : team_data.members) {
						member->GetMachine()->is_attacking = true;
					}
					if (!damaged_bool)
						state_machine->is_attacking = true;
				}
			}
			//ほかのグループと押し合う
			Vector3 avoid = { 0,0,0 };
			for (auto& group : another_groups) {
				if (group == &team_data)
					continue;
				Vector3 to_other = group->team_position - team_data.team_position;
				if (to_other.magnitudeSquared() <= 8.0f * 8.0f) {
					if (to_other.magnitudeSquared() <= 1e-6f * 1e-6f)
						to_other = { Random::Float01() + 0.01f,0.0f,Random::Float01() + 0.01f };
					avoid -= group->team_mov_vec.getNormalized() / to_other.magnitudeSquared() * 50.0f;
				}
			}
			team_data.team_mov_vec = team_data.team_mov_vec * 5 + avoid;
		}
		if (state_machine->enemy->transform->position.y < -50)
			Die();
		//空のオブジェクトが持つ場合は自身のstate_machineのことは考慮しない
		if (!state_machine) {
			if (team_data.member_num > 0)
				team_data.team_position = team_data.members[0]->GetOwnerBody()->transform->position;
			return;
		}
		team_data.team_position = body->transform->position;


		const float dt = Time::UnscaledDeltaTime();
		if (i_frame_timer > 0.0f)
			i_frame_timer -= dt;

		state_machine->is_damaged = damaged_bool;
		state_machine->knock_back = knock_back_bool;
		state_machine->is_dead = hp == 0;

		if (hp != 0 && !knock_back_bool)
			state_machine->move_vec = team_data.team_mov_vec;

	}
	void LeaderEnemyBrain::Damage(u32 damage, bool ignore_i_frame)
	{
		if (state_machine) {
			state_machine->is_damaged = true;
			if (damage >= hp) {
				Die();
				return;
			}
			hp -= damage;

			i_frame_timer = I_FRAME;
			is_damaged = true;

		}
	}
	void LeaderEnemyBrain::Die() {
		hp = 0;
		u64 member_num = team_data.members.size();
		if (member_num == 0)
			return;
		for (auto& member : team_data.members) {
			member->AddLeader(nullptr);
		}


		auto ite = std::find(another_groups.begin(), another_groups.end(), &team_data);
		another_groups.erase(ite);

		//仮で、群れの中から次のリーダーを選出するようにする
		//TODO: 確率でもいいので、他の群れに合流するとかあってもいいかも
		if (member_num > 3 || another_groups.size() <= 1) {
			u64 next_leader = Random::Int(0, member_num - 1);
			auto next_leader_brain = make_safe_unique<LeaderEnemyBrain>(team_data.members[next_leader]->GetMachine(), player);
			for (u64 member_idx = 0; member_idx < team_data.members.size(); member_idx++) {
				if (member_idx == next_leader)
					continue;
				auto* member_brain = team_data.members[member_idx];
				member_brain->AddLeader(next_leader_brain.get());
			}
			team_data.members[next_leader]->GetOwnerBody()->enem_controller->SetBrain(std::move(next_leader_brain));
			team_data.members.clear();
		}
		else {
			u64 next_team_idx = Random::Int(0, another_groups.size() - 1);

			auto next_leader_brain = another_groups[next_team_idx]->leader;
			for (u64 member_idx = 0; member_idx < team_data.members.size(); member_idx++) {
				auto* member_brain = team_data.members[member_idx];
				member_brain->AddLeader(next_leader_brain);
			}
			team_data.members.clear();
		}




	}
	void LeaderEnemyBrain::DebugDraw()
	{
		DrawCylinder3D(cast(state_machine->enemy->transform->position), cast(state_machine->enemy->transform->position + Vector3(0, 10, 0)), 8, 16, Color::YELLOW, Color::YELLOW, false);
		DrawLine3D(cast(state_machine->enemy->transform->position + Vector3(0, 5, 0)), cast(next_target), Color::RED);
	}
	void LeaderEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		if (state_machine) {
			knock_back = true;
			state_machine->move_vec = knock_back_vec;
		}
	}
}