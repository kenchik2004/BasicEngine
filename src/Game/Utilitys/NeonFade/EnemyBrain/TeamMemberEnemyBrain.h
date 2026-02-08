#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {

	class EnemyStateMachine;
	class LeaderEnemyBrain;
	class EnemyTeam;
	class TeamMemberEnemyBrain :
		public AbstractEnemyBrain
	{
	public:
		TeamMemberEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team);
		~TeamMemberEnemyBrain();
		void Think() override;
		void AddLeader(LeaderEnemyBrain* new_leader);
		void Damage(u32 damage = 0, bool ignore_i_frame = false) override;
		void Die();
		void KnockBack(Vector3 knock_back_vec) override;
		void DebugDraw() override;
		void RemoveFromTeam();

	private:
		EnemyStateMachine* machine = nullptr;
		EnemyTeam* my_team = nullptr;
		float i_frame_timer = 0.0f;
		static constexpr float I_FRAME = 0.05f;
		trigger_bool knock_back = false;
		trigger_bool is_damaged = false;
		std::unordered_map<std::string, std::function<void()>> thinkings;
		std::function<std::string()> update_selector;
		u32 hp;
		static constexpr u32 MAX_HP = 100;
	};
}

