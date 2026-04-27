//---------------------------------------------------------------------------
//! @file   EnemyStateMachine.h
//! @brief  敵キャラクター用ステートマシンクラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "IStateMachine.h"
namespace NeonFade {
	class Enemy;
	class EnemyStateMachine :
		public IStateMachine
	{
	public:
		EnemyStateMachine(Enemy* owner_);
		void DebugDraw() override;
		void OnTriggerEnter(const HitInfo& hit_info) override;
		virtual ~EnemyStateMachine();
		Enemy* enemy = nullptr;
		trigger_bool is_damaged = false;
		trigger_bool knock_back = false;
		bool is_dead = false;
		trigger_bool is_attacking = false;
		trigger_bool is_instructing = false;
		bool is_confused = false;
		bool is_escaping = false;
		trigger_bool is_step_back = false;
		Vector3 move_vec = { 0,0,0 };
	private:
	};
}

