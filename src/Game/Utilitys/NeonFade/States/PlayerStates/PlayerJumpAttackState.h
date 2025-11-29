#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	class Player;
	class Enemy;
	class PlayerJumpAttackState :
		public IState
	{
	public:
		PlayerJumpAttackState(Player* player_);
		void OnEnter(IStateMachine* machine);
		void OnExit(IStateMachine* machine);
		void Update(IStateMachine* machine, float dt);
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info)override;
		void DebugDraw() override;
	private:
		float exit_timer = 0.0f;
		bool atk_main_started = false;
		static constexpr float exit_time = 0.8f;
		float attack_interval_timer = 0.0f;
		static constexpr float attack_interval = 0.05f;
		Player* owner_player;
		Enemy* target = nullptr;
		RigidBody* rb;
		Animator* animator;
		SphereCollider* hit_box = nullptr;
		SafeSharedPtr<BoxCollider> atk_range_trigger = nullptr;
	};

}
