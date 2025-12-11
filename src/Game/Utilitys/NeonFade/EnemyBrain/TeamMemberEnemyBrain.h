#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {

	class EnemyStateMachine;
	class LeaderEnemyBrain;
	class TeamMemberEnemyBrain :
		public AbstractEnemyBrain
	{
	public:
		TeamMemberEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_);
		void Think() override;
		void AddLeader(LeaderEnemyBrain* new_leader);
		void Damage(u32 damage = 0, bool ignore_i_frame = false) override;
		void Die();
		void KnockBack(Vector3 knock_back_vec) override;
		void DebugDraw() override;

	private:
		u32 number_in_team = 0;
		LeaderEnemyBrain* leader = nullptr;

		Vector3 input_vec = { 0,0,0 };
		float i_frame_timer = 0.0f;
		static constexpr float I_FRAME = 0.05f;
		trigger_bool knock_back = false;
		trigger_bool is_damaged = false;
	};
}

