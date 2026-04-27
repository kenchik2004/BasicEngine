//---------------------------------------------------------------------------
//! @file   EnemyTeam.h
//! @brief  敵チームの管理・連携処理クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
namespace NeonFade {
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
			if (leader_brain)
				leader_brain->RemoveFromTeam();
			for (; members.size() > 0;) {
				auto& member = members[0];
				if (member) {
					member->RemoveFromTeam();
				}
			}
		}
	};
}

