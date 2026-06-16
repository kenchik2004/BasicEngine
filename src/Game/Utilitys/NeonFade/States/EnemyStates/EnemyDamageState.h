#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Enemy;
	class EnemyDamageState :
		public IState
	{
	public:
		EnemyDamageState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		bool CanTransitTo(const std::string& state_name) override;

	private:
		static constexpr float DAMAGE_DURATION = 0.5f;
		float elapsed_time = 0.0f;
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
	};
}
