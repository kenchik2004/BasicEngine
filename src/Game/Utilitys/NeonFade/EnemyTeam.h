#pragma once
namespace NeonFade {
	class LeaderEnemyBrain;
	class TeamMemberEnemyBrain;
	class EnemyTeam
	{
	private:
		LeaderEnemyBrain* leader_brain;
		std::vector<TeamMemberEnemyBrain*> members;
	public:
		u64 GetMemberNum() const {
			return members.size();
		}
		LeaderEnemyBrain* GetLeader() const {
			return leader_brain;
		}
		void AddMember(TeamMemberEnemyBrain* member) {
			members.push_back(member);
		}
		void SetLeader(LeaderEnemyBrain* leader) {
			leader_brain = leader;
		}
		void SubMember(TeamMemberEnemyBrain* member) {
			members.erase(std::remove(members.begin(), members.end(), member), members.end());
		}
		void ClearTeam() {
			leader_brain = nullptr;
			members.clear();
		}
	};
}

