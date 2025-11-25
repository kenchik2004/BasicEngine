#pragma once
#include "Game/Utilitys/NeonFade/StateMachines/PlayerStateMachine.h"
namespace NeonFade
{
	USING_PTR(PlayerStateMachine);
	class PlayerController :
		public Component
	{
	public:
		USING_SUPER(PlayerController);
		int Init() override;
		void Update() override;
		void Exit() override;
		void LateDebugDraw() override;
		void PrePhysics() override;

		void Damage(int damage);

		void OnCollisionEnter(const HitInfo& hit_info);
		void OnCollisionStay(const HitInfo& hit_info);
		void OnCollisionExit(const HitInfo& hit_info);
		void OnTriggerEnter(const HitInfo& hit_info);
		void OnTriggerStay(const HitInfo& hit_info);
		void OnTriggerExit(const HitInfo& hit_info);

	private:
		PlayerStateMachineUP state_machine;
		SafeSharedPtr<Player> owner_player = nullptr;
		bool is_jumping = false;
		bool is_landed = false;
		bool is_falling = false;
		bool is_attacking = false;
		bool is_dodging = false;

		bool is_damaged = false;
		float fall_detect_time = 0.0f;
		constexpr static float FALL_DETECT_THRESHOLD = 0.4f;
		bool can_climb = false;
		Vector3 climb_hit_point = { 0,0,0 };
		Vector3 climb_hit_normal = { 0,0,0 };
		Vector3 climb_ray_start = { 0,0,0 };
		bool is_climbing = false;
		Vector3 move_input = { 0,0,0 };
	};
}

