#pragma once
namespace NeonFade {
	USING_PTR(EnemyStateMachine);
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

		void Damage();
		void Down(Vector3 vec);
	private:
		EnemyStateMachineUP state_machine;
	};
}

