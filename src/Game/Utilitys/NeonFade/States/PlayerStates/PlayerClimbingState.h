#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Player;
	class PlayerClimbingState :
		public IState
	{
	public:
		PlayerClimbingState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info)override {};
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {};
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {};

		Vector3 climb_start_pos;
		Vector3 climb_end_pos;
	private:
		Player* owner_player;
		RigidBody* rb;
		Animator* animator;
		float climb_time = 1.5f;
	};
}

