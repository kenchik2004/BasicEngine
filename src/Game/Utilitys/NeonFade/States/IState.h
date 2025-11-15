#pragma once

class GameObject;
namespace NeonFade
{
	class IStateMachine;
	class IState
	{
	public:
		virtual ~IState() = default;
		IState(GameObject* owner_);
		virtual void OnEnter(IStateMachine* machine) = 0;
		virtual void OnExit(IStateMachine* machine) = 0;
		virtual void Update(IStateMachine* machine, float dt) = 0;
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) {};
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) {};
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) {};
		virtual void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) {};
		virtual void OnTriggerStay(IStateMachine* machine, const HitInfo& hit_info) {};
		virtual void OnTriggerExit(IStateMachine* machine, const HitInfo& hit_info) {};
		const std::string& GetName() { return name; }
		void SetName(const std::string_view name_) { name = name_; }
		virtual void DebugDraw() {};
		std::string GetChangeRequest();
		void RegisterChangeRequest(const std::string& state_name, std::function<bool()>& func, u8 priority = 0);
	private:
		GameObject* owner = nullptr;
		std::string name;
		struct FunctionRequest {
			u8 priority;
			std::string state_name;
			std::function<bool()> function = nullptr;
		};
		std::vector <SafeUniquePtr<FunctionRequest>> change_requests;
	};
}