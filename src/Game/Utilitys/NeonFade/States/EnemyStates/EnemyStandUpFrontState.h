#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyStandUpFrontState :
		public IState
	{
	public:
		EnemyStandUpFrontState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		bool CanTransitTo(const std::string& state_name) override;

	private:
		void RotateCollider();
		void TranslateCollider();
		static constexpr float STAND_UP_DURATION = 6.0f;
		float elapsed_time = 0.0f;
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		CapsuleCollider* col = nullptr;
	};
}

