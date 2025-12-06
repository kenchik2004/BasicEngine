#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class PlayerAttack1State :
		public IState
	{
	public:
		PlayerAttack1State(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)override;
		static constexpr float EXIT_TIME = 0.7f;
		static constexpr float SPIN_TIME = 0.4f;
		float exit_timer = 0.0f;
	private:
		Player* owner_player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		SafeSharedPtr<Collider> hit_box = nullptr;

		Quaternion spin_rot = Quaternion(physx::PxIdentity);
		Quaternion start_rot = Quaternion(physx::PxIdentity);
	};
}

