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
		another_groups.push_back(&team_data);
	}
	LeaderEnemyBrain::~LeaderEnemyBrain()
	{
		auto ite = std::find(another_groups.begin(), another_groups.end(), &team_data);
		another_groups.erase(ite);
	}
	void LeaderEnemyBrain::Think()
	{



		//ここで思考する
		{
			if (!player)
				return;
			Vector3 pl_dist = player->transform->position - team_data.team_position;
			pl_dist.y = 0;//高さ方向は無視
			//一旦仮で、プレイヤーから一定距離内に近付き、その中でランダムウォークを行うようにしておく
			if (pl_dist.magnitudeSquared() > 50.0f * 50.0f) {
				team_data.team_mov_vec = pl_dist.getNormalized();
				next_target = Random::Position(team_data.team_position - Vector3(30, 0, 30), team_data.team_position + Vector3(30, 0, 30));
			}
			else {
				Vector3 random_walk_vec = next_target - team_data.team_position;
				if (random_walk_vec.magnitudeSquared() <= 1.0f)
					next_target = Random::Position(team_data.team_position - Vector3(30, 0, 30), team_data.team_position + Vector3(30, 0, 30));
				random_walk_vec = next_target - team_data.team_position;
				team_data.team_mov_vec = random_walk_vec.getNormalized();
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
					avoid -= (to_other.getNormalized() * 8.0f / to_other.magnitude());
				}
			}
			team_data.team_mov_vec = team_data.team_mov_vec*5  + avoid;
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

		bool knock_back_bool = knock_back;
		state_machine->is_damaged = is_damaged;
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
		//仮で、群れの中から次のリーダーを選出するようにする
		//TODO: 確率でもいいので、他の群れに合流するとかあってもいいかも
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
	void LeaderEnemyBrain::DebugDraw()
	{
		DrawCylinder3D(cast(state_machine->enemy->transform->position), cast(state_machine->enemy->transform->position + Vector3(0, 10, 0)), 8, 16, Color::YELLOW, Color::YELLOW, false);

	}
	void LeaderEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		if (state_machine) {
			knock_back = true;
			state_machine->move_vec = knock_back_vec;
		}
	}
}