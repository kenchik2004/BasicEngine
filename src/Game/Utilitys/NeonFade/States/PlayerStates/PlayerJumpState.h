#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

class RigidBody;
class Animator;
namespace NeonFade
{
	class Player;
	class PlayerJumpState :
		public IState
	{
	public:
		PlayerJumpState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override {}
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		float jump_timer = 0.0f;
		static constexpr float JUMP_TIME = 0.4f;
	private:
		Player* player = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
	};
}

