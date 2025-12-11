#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {
	class Enemy;
	class EnemyStateMachine;
	class TeamMemberEnemyBrain;
	class LeaderEnemyBrain :
		public AbstractEnemyBrain
	{
	public:
		LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_);
		~LeaderEnemyBrain();
		void Think() override;
		void Damage(u32 damage = 0, bool ignore_i_frame = false) override;
		void KnockBack(Vector3 knock_back_vec) override;
		void Die();
		void DebugDraw() override;
		u32 AddMember(TeamMemberEnemyBrain* new_member) {
			if (!new_member)
				return 0;
			team_data.members.push_back(new_member);
			return ++team_data.member_num;
		}
		void SubstractMember(TeamMemberEnemyBrain* delete_member) {
			if (!delete_member)
				return;
			auto it = std::find(team_data.members.begin(), team_data.members.end(), delete_member);
			if (it == team_data.members.end())
				return;
			team_data.members.erase(it);
			team_data.member_num--;
		}

		struct LeaderBrainData {
			Vector3 team_position = { 0,0,0 };
			Vector3 team_mov_vec = { 0,0,1 };
			u32 member_num = 0;
			std::vector<TeamMemberEnemyBrain*> members;
		};
		const LeaderBrainData& GetTeamData() const { return team_data; }
	private:
		LeaderBrainData team_data;
		Vector3 next_target = { 0,0,0 };
		static inline std::vector<LeaderBrainData*> another_groups = std::vector<LeaderBrainData*>(0);

		//オブジェクトに持たせる用の変数
		float i_frame_timer = 0.0f;
		static constexpr float I_FRAME = 0.05f;
		trigger_bool knock_back = false;
		trigger_bool is_damaged = false;
	};
}

