//---------------------------------------------------------------------------
//! @file   EnemyStateMachine.cpp
//! @brief  EnemyStateMachineの実装。敵キャラクターのステートマシン処理を行う
//---------------------------------------------------------------------------
#include "precompile.h"
#include "EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	EnemyStateMachine::EnemyStateMachine(Enemy* owner_)
		:IStateMachine(static_cast<GameObject*>(owner_))
	{
		enemy = owner_;
	}

	void EnemyStateMachine::OnTriggerEnter(const HitInfo& hit_info)
	{
		if (current_state)
			current_state->OnTriggerEnter(this, hit_info);
	}
	EnemyStateMachine::~EnemyStateMachine()
	{}
}
