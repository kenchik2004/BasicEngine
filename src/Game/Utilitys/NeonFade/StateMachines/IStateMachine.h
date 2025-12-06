#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h" 
class GameObject;
namespace NeonFade
{
	class IStateMachine
	{
	public:
		virtual ~IStateMachine() = default;
		IStateMachine(GameObject* owner_);

		virtual void OnSelect();
		virtual void OnDeselect();

		virtual void ChangeState(std::string_view next);
		std::string GetCurrentStateName() { return current_state ? current_state->GetName() : ""; }
		virtual void Update(float dt);
		void AddState(std::string_view name, SafeUniquePtr<IState> state);
		virtual void OnCollisionEnter(const HitInfo& hit_info);
		virtual void OnCollisionStay(const HitInfo& hit_info);
		virtual void OnCollisionExit(const HitInfo& hit_info);
		virtual void OnTriggerEnter(const HitInfo& hit_info);
		virtual void OnTriggerStay(const HitInfo& hit_info);
		virtual void OnTriggerExit(const HitInfo& hit_info);
		virtual void DebugDraw();
	protected:
		IState* current_state = nullptr;
		GameObject* owner = nullptr;
		std::unordered_map<std::string, SafeUniquePtr<IState>> states;
	};
}

