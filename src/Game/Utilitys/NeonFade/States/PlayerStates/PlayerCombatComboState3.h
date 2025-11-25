#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {
	class Player;
	class PlayerCombatComboState3 :
		public IState
	{
	public:
		PlayerCombatComboState3(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
	private:
		Player* owner_player = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		bool input_limit = false;
		bool next_avalable = false;
		bool knock_back = false;
		SafeWeakPtr<SphereCollider> hit_box;
		SafeWeakPtr<class Enemy> target;
		float hit_stop_timer = 0.0f;
		static constexpr float HIT_STOP_TIME = 0.1f;
	};
}

