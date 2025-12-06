#pragma once
#include "Game/Utilitys/NeonFade/StateMachines/IStateMachine.h"
namespace NeonFade {
	USING_PTR(EntityController);
	class EntityController :
		public Component
	{
	public:
		USING_SUPER(EntityController);
		int Init() override;
		void Update() override;
		void OnCollisionEnter(const HitInfo& hit_info);
		void OnCollisionStay(const HitInfo& hit_info);
		void OnCollisionExit(const HitInfo& hit_info);
		void OnTriggerEnter(const HitInfo& hit_info);
		void OnTriggerStay(const HitInfo& hit_info);
		void OnTriggerExit(const HitInfo& hit_info);
		const Vector3& GetInputVector() const { return input_vector; }
		void SwapMachine(const std::string& name);

	private:
		std::unordered_map<std::string, SafeUniquePtr<IStateMachine>> state_machines;
		IStateMachine* current_machine = nullptr;
		Vector3 input_vector = Vector3(0, 0, 0);



	};
}

