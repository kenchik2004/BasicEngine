#pragma once
#include "IState.h"
namespace NeonFade {

	class Player;
	class PlayerJumpAttackLandState :
		public IState
	{
	public:
		PlayerJumpAttackLandState(Player* owner_);
		void OnEnter(IStateMachine* machine);
		void OnExit(IStateMachine* machine);
		void Update(IStateMachine* machine, float dt);
	private:

		Player* owner_player;
		RigidBody* rb;
		Animator* animator;
	};
}

