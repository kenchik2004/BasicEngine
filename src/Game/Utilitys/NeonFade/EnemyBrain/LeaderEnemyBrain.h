//---------------------------------------------------------------------------
//! @file   LeaderEnemyBrain.h
//! @brief  リーダー敵AI思考クラスの定義
//---------------------------------------------------------------------------
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
		void RemoveFromTeam();


	private:
		EnemyTeam* my_team = nullptr;
		EnemyStateMachine* machine = nullptr;

		std::unordered_map<std::string, std::function<void()>> thinkings;
		std::function<std::string()> update_selector;
		u32 hp;
		static constexpr u32 MAX_HP = 300;
		float i_frame_timer = 0.0f;
		static constexpr float I_FRAME = 0.1f;
		trigger_bool knock_back = false;
		trigger_bool is_damaged = false;
	};
}

