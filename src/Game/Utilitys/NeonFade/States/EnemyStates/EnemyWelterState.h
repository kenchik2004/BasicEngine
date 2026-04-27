//---------------------------------------------------------------------------
//! @file   EnemyWelterState.h
//! @brief  敵のよろめき状態クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Enemy;
	class EnemyWelterState :
		public IState
	{
	public:
		EnemyWelterState(Enemy* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
	private:
		Enemy* enemy = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		SafeSharedPtr<AudioClip> welter_se;
		float timer = 0.0f;
		static constexpr float WELTER_TIME = 3.0f;
	};
}

