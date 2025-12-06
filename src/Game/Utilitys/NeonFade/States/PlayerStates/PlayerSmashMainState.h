#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class PlayerSmashMainState :
		public IState
	{
	public:
		PlayerSmashMainState(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void DebugDraw() override;
		float smash_timer = 0.0f;
		const float MAX_SMASH_TIME = 1.5f;
	private:
		Player* owner_player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		bool anim_after_smash = false;
		SafeSharedPtr<Collider> hit_box = nullptr;

	};
}

