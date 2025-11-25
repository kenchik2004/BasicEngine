#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Enemy;
	class EnemyDieState :
		public IState
	{
	public:
		EnemyDieState(Enemy* owner_);
		~EnemyDieState() override;
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
	private:
		Enemy* owner_enemy = nullptr;
		Animator* animator = nullptr;
		ModelRenderer* model = nullptr;
		float exit_timer = 0;
		static constexpr float EXIT_TIME = 1.0f;
	};
}

