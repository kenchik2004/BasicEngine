//---------------------------------------------------------------------------
//! @file   BasicEnemyBrain.h
//! @brief  基本的な敵AI思考クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {
	class EnemyStateMachine;
	class BasicEnemyBrain :
		public AbstractEnemyBrain
	{
	public:
		BasicEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_);
		std::string Think() override;
		void Damage(u32 damage = 0, bool ignore_i_frame = false) override;
		void Die();
		void KnockBack(Vector3 knock_back_vec) override;
		void DebugDraw() override;

	private:
		//! @briref Think関数内でのリターン前に呼び出して、ダメージやノックバックの状態をリセットするための関数
		void ResetParameters();

		bool IsFoundPlayer();

	};
}

