//---------------------------------------------------------------------------
//! @file   EnemyLeaderAttackState.h
//! @brief  リーダー敵の攻撃状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyLeaderAttackState :
		public IState
	{
	public:
		EnemyLeaderAttackState(Enemy* owner_, GameObjectWP player_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
	public:
		float attack_timer = 0.0f;
		const float ATTACK_TIME = 3.0f;
	private:
		Enemy* enemy;
		RigidBody* rb;
		Animator* animator;
		GameObjectWP player;
	};
}


