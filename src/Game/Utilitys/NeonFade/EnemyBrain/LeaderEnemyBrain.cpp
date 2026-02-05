#include "LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyTeam.h"

namespace NeonFade {
	LeaderEnemyBrain::LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		my_team = team;
		machine = state_machine_;
	}
	LeaderEnemyBrain::~LeaderEnemyBrain()
	{
		if (my_team)
			my_team->SetLeader(nullptr);
	}
	void LeaderEnemyBrain::Think()
	{

	}
	void LeaderEnemyBrain::Damage(u32 damage, bool ignore_i_frame)
	{
		machine->is_dead = true;
	}
	void LeaderEnemyBrain::Die() {
	}
	void LeaderEnemyBrain::DebugDraw()
	{
	}
	void LeaderEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
	}
}