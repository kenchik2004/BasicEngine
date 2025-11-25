#pragma once
namespace NeonFade {

	class ISceneStateMachine;
	class ISceneState
	{
	public:
		virtual ~ISceneState() = default;
		ISceneState(Scene* owner_scene_);
		virtual void OnEnter(ISceneStateMachine* machine) = 0;
		virtual void OnExit(ISceneStateMachine* machine) = 0;
		virtual void Update(ISceneStateMachine* machine, float dt) = 0;
		const std::string& GetName() { return name; }
		void SetName(const std::string_view name_) { name = name_; }
		virtual void DebugDraw() {};
		std::string GetChangeRequest();
		void RegisterChangeRequest(const std::string& state_name, std::function<bool()>& func, u8 priority = 0);
	private:
		Scene* owner_scene = nullptr;
		std::string name;
		struct FunctionRequest {
			u8 priority;
			std::string state_name;
			std::function<bool()> function = nullptr;
		};
		std::vector <SafeUniquePtr<FunctionRequest>> change_requests;
	};
}

