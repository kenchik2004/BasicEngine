//---------------------------------------------------------------------------
//! @file   EnemyEscapeState.h
//! @brief  敵の逃走状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Enemy;
	class EnemyEscapeState :
		public IState
	{
	public:
		EnemyEscapeState(Enemy* owner_, SafeWeakPtr<Object> leader_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
	private:
		Enemy* enemy = nullptr;
		SafeWeakPtr<Object> leader = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		GameObject* player = nullptr;
		float escape_timer = 0.0f;
		static constexpr float ESCAPE_TIME = 5.0f;
		bool exit_flag = false;
	};
}

