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
		bool IsWeakened() const { return is_weakened; }

	private:
		static constexpr u32 WEAKED_HP_THRESHOLD = 30;	//<! HPがこの値以下のとき、弱っている状態とみなす
		bool is_weakened = false;						//<! 敵が弱っている状態かどうかを示すフラグ
		//! @briref Think関数内でのリターン前に呼び出して、ダメージやノックバックの状態(1フレーム単位で管理する物)をリセットするための関数
		void ResetFrameParameters();

		bool IsFoundPlayer();
		bool IsHaveToCoverOtherEnemy();

	};
}

