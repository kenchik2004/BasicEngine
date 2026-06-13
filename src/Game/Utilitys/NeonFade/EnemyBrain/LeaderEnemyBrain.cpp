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

namespace NeonFade
{
	LeaderEnemyBrain::LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		my_team = team;
		machine = state_machine_;
		player = player_;
		hp = MAX_HP;
		
	}
	LeaderEnemyBrain::~LeaderEnemyBrain()
	{
		RemoveFromTeam();
	}
	void LeaderEnemyBrain::Think()
	{
		i_frame_timer -= Time::DeltaTime();
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