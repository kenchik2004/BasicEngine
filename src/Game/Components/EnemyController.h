#pragma once
namespace NeonFade {
	USING_PTR(EnemyStateMachine);
	USING_PTR(Player);
	class EnemyController :
		public Component
	{
	public:
		USING_SUPER(EnemyController);
		int Init() override;
		void Update() override;
		void Exit() override;
		void LateDebugDraw() override;

		void OnCollisionEnter(const HitInfo& hit_info);
		void OnCollisionStay(const HitInfo& hit_info);
		void OnCollisionExit(const HitInfo& hit_info);
		void OnTriggerEnter(const HitInfo& hit_info);
		void OnTriggerStay(const HitInfo& hit_info);
		void OnTriggerExit(const HitInfo& hit_info);

		void Damage(int damage, bool ignore_i_frame = false);
		void Down(Vector3 vec, int damage = 1);
		Vector3 move_vec = { 0,0,0 };
		int hp = 10;
		float invincible_timer = 0;
		static constexpr float INVINCIBLE_TIME = 0.1f;
		bool is_damaged = false;
		bool is_dead = false;
		bool is_attacking = false;
		bool knock_back = false;
		Vector3 knock_back_vec = { 0,0,0 };
	private:
		EnemyStateMachineUP state_machine;
		PlayerWP player = nullptr;

	};
}

