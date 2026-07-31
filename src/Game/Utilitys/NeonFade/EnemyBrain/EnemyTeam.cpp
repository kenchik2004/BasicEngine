//---------------------------------------------------------------------------
//! @file   EnemyTeam.cpp
//! @brief  EnemyTeamの実装。敵チームの管理・連携処理を行う
//---------------------------------------------------------------------------
#include "EnemyTeam.h"


namespace NeonFade
{
	//! @brief チームを全てのチームのポインタを保持する静的変数に登録する
	void EnemyTeam::RegisterTeam()
	{
		all_teams.push_back(this);
	}
	//! @brief チームを全てのチームのポインタを保持する静的変数から削除する
	void EnemyTeam::UnregisterTeam()
	{
		all_teams.erase(std::remove(all_teams.begin(), all_teams.end(), this), all_teams.end());
	}
	EnemyTeam::EnemyTeam()
	{
		// チームを全てのチームのポインタを保持する静的変数に登録する
		RegisterTeam();
	}
	EnemyTeam::~EnemyTeam()
	{
		//ここでポインタを直接nullptrにしないようにする
		//直接削除してしまうと、リーダーやメンバー側にこのチームのポインタが残ってしまい、アクセス違反が発生する可能性がある
		//ClearTeam()を呼び出して、リーダーやメンバー側からチームのポインタも同時に削除するようにする
		ClearTeam();
		// チームを全てのチームのポインタを保持する静的変数から削除する
		UnregisterTeam();


	}


	void EnemyTeam::AddMember(TeamMemberEnemyBrain* member) {
		// メンバーがnullptrの場合は追加しない
		if (!member)
			return;

		// すでにメンバーとして登録されている場合は追加しない
		if (std::find(members.begin(), members.end(), member) != members.end())
			return;

		// メンバーとして登録する
		members.push_back(member);
		member->SetTeamRef(this);

	}


	void EnemyTeam::SetLeader(LeaderEnemyBrain* leader) {
		// 既存のリーダーがいる場合は、チームの参照を削除する
		if (leader_brain)
			leader_brain->ResetTeamRef();

		leader_brain = leader;
		if (leader_brain)
			leader_brain->SetTeamRef(this);
	}


	void EnemyTeam::SubMember(TeamMemberEnemyBrain* member) {
		// メンバーとして登録されている場合は削除する
		if (std::find(members.begin(), members.end(), member) != members.end()) {
			members.erase(std::remove(members.begin(), members.end(), member), members.end());
			// メンバーのチーム参照を削除する
			if (member)
				member->ResetTeamRef();
		}
	}


	void EnemyTeam::ClearTeam() {

		// チームのリーダーとメンバーをチームから削除する

		// リーダーが存在する場合は、リーダーをチームから削除する
		if (leader_brain) {
			leader_brain->ResetTeamRef();
			leader_brain = nullptr;
		}

		// メンバーが存在する場合は、全メンバーをチームから削除する
		while (members.size() > 0) {

			auto& member = members[0];
			if (member) {
				// メンバーのチーム参照を削除する
				member->ResetTeamRef();
				// メンバーをチームから削除する
				members.erase(members.begin());
			}
		}
	}
}
