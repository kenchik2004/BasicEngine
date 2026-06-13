//---------------------------------------------------------------------------
//! @file   TeamMemberEnemyBrain.cpp
//! @brief  TeamMemberEnemyBrainの実装。チームメンバー敵のAI思考ロジックを実装する
//---------------------------------------------------------------------------
#include "TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/PlayerController.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyTeam.h"

namespace NeonFade {

	TeamMemberEnemyBrain::TeamMemberEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		machine = state_machine_;
		my_team = team;
		hp = MAX_HP;


	}

	TeamMemberEnemyBrain::~TeamMemberEnemyBrain()
	{
		if (my_team) {
			my_team->SubMember(this);
		}
	}

	void TeamMemberEnemyBrain::Think()
	{

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