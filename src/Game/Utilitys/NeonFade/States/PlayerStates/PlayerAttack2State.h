#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class PlayerAttack2State :
		public IState
	{
	public:
		PlayerAttack2State(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)override;
		static constexpr float EXIT_TIME = 0.8f;
		float exit_timer = 0.0f;
		float hit_stop_timer = 0.0f;
		static constexpr float HIT_STOP_TIME = 0.1f;
	private:
		Player* owner_player = nullptr;
		Animator* animator = nullptr;
		RigidBody* rb = nullptr;
		SafeSharedPtr<BoxCollider> hit_box = nullptr;
		bool effect_played = false;
	};
}

