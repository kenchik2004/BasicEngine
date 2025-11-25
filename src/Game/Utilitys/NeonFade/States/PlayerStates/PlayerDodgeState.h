#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Player;
	class PlayerDodgeState :
		public IState
	{
	public:
		PlayerDodgeState(Player* owner_);
		virtual void OnEnter(IStateMachine* machine) override;
		virtual void OnExit(IStateMachine* machine) override;
		virtual void Update(IStateMachine* machine, float dt) override;
		float dodge_timer = 0;
		static constexpr float DODGE_TIME = 0.2f;
		static constexpr float I_FRAME_TIME = 0.1f;
		static constexpr float DODGE_SPEED = 50.0f;
	private:
		Player* player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
	};
}

