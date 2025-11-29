#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	class Player;
	class PlayerDamageState :
		public IState
	{
	public:
		PlayerDamageState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		float exit_timer = 0;
		static constexpr float EXIT_TIME = 0.5f;
	private:
		Player* player = nullptr;
		Animator* animator = nullptr;
	};
}

