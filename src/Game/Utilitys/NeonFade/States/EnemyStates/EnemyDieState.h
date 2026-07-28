#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Enemy;
	class EnemyDieState :
		public IState
	{
	public:
		EnemyDieState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		bool CanTransitTo(const std::string& state_name) override;

	private:
		static constexpr float DIE_DURATION = 2.5f;
		float elapsed_time = 0.0f;
		Enemy* owner_enemy = nullptr;
		ModelRenderer* model = nullptr;
		Animator* animator = nullptr;
		Material* death_material = nullptr;
		Collider* collider = nullptr;
		RigidBody* rb = nullptr;
	};
}
