//---------------------------------------------------------------------------
//! @file   AbstractEnemyBrain.cpp
//! @brief  AbstractEnemyBrainの実装。敵AIの基底となる思考処理を定義する
//---------------------------------------------------------------------------
#include "AbstractEnemyBrain.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

namespace NeonFade {

	AbstractEnemyBrain::AbstractEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		: state_machine(state_machine_), player(player_) {

		if (state_machine_)
			body = state_machine_->enemy;
	}
	AbstractEnemyBrain::~AbstractEnemyBrain()
	{
	}
	
}

