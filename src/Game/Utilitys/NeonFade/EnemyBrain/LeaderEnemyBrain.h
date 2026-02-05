#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {
	class Enemy;
	class EnemyStateMachine;
	class TeamMemberEnemyBrain;
	class EnemyTeam;
	class LeaderEnemyBrain :
		public AbstractEnemyBrain
	{
	public:
		LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team);
		~LeaderEnemyBrain();
		void Think() override;
		void Damage(u32 damage = 0, bool ignore_i_frame = false) override;
		void KnockBack(Vector3 knock_back_vec) override;
		void Die();
		void DebugDraw() override;


	private:
		EnemyTeam* my_team = nullptr;
		EnemyStateMachine* machine = nullptr;
	};
}

