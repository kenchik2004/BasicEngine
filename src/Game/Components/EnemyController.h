#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"
namespace NeonFade {
	USING_PTR(EnemyStateMachine);
	USING_PTR(Player);
	USING_PTR(AbstractEnemyBrain);
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
		void Down(Vector3 vec);
		bool IsDead();
		u32 GetHp();
		EnemyStateMachine* GetStateMachine() { return state_machine.get(); }
		AbstractEnemyBrain* GetBrain() { return brain.get(); }
		void SetPlayer(PlayerWP player_) { player = player_; }
		void SetBrain(AbstractEnemyBrainUP new_brain) {
			brain = std::move(new_brain); brain->Think();
		}
	private:
		EnemyStateMachineUP state_machine;
		AbstractEnemyBrainUP brain = nullptr;
		PlayerWP player = nullptr;
		bool is_leader = false;
	};

}

