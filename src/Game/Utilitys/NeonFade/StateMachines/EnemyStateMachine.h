//---------------------------------------------------------------------------
//! @file   EnemyStateMachine.h
//! @brief  敵キャラクター用ステートマシンクラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "IStateMachine.h"
namespace NeonFade {
	class Enemy;
	class AbstractEnemyBrain;
	class EnemyStateMachine :
		public IStateMachine
	{
	public:
		EnemyStateMachine(Enemy* owner_);
		void OnTriggerEnter(const HitInfo& hit_info) override;
		virtual ~EnemyStateMachine();

		void SetBrain(AbstractEnemyBrain* new_brain) { brain = new_brain; }
		AbstractEnemyBrain* GetBrain() { return brain; }

		Enemy* enemy = nullptr;
	private:
		AbstractEnemyBrain* brain = nullptr;
	};
}

