#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class PlayerAttack3State :
		public IState
	{
	public:
		PlayerAttack3State(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)override;
		static constexpr float EXIT_TIME = 0.6f;
		float exit_timer = 0.0f;
		float hit_stop_timer = 0.0f;
		static constexpr float HIT_STOP_TIME = 0.2f;
	private:
		float hit_box_created_time = 0.0f;
		Player* owner_player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		SafeSharedPtr<Collider> hit_box = nullptr;
	};
}

