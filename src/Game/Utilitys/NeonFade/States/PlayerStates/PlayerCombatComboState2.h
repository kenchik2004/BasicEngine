#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class Enemy;
	class PlayerCombatComboState2 :
		public IState
	{
	public:
		PlayerCombatComboState2(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
	private:
		Player* player = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		bool input_limit = false;
		bool next_avalable = false;
		bool knock_back = false;
		SafeWeakPtr<SphereCollider> hit_box;
		SafeWeakPtr<Enemy> target;
		float attack_timer = 0.0f;
		static constexpr float ATTACK_TIME = 0.65f;
		float hit_stop_timer = 0.0f;
		static constexpr float HIT_STOP_TIME = 0.03f;
	};
}

