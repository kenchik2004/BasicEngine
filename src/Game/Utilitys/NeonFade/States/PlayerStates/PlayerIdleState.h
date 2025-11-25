#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

class RigidBody;
class Animator;
namespace NeonFade
{
	class Player;
	class PlayerIdleState :
		public IState
	{
	public:
		PlayerIdleState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override {}
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}

	private:
		Player* player = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
	};
}

