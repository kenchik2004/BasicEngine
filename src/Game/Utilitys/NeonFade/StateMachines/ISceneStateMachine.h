#pragma once
#include "Game/Utilitys/NeonFade/States/ISceneState.h"
namespace NeonFade {

	class ISceneStateMachine
	{
	public:

		virtual ~ISceneStateMachine() = default;
		ISceneStateMachine(Scene* owner_scene_);
		virtual void ChangeState(std::string_view next);
		std::string GetCurrentStateName() { return current_state ? current_state->GetName() : ""; }
		virtual void Update(float dt);
		void AddState(std::string_view name, SafeUniquePtr<ISceneState> state);
	private:
		ISceneState* current_state = nullptr;
		Scene* owner_scene = nullptr;
		std::unordered_map<std::string, SafeUniquePtr<ISceneState>> states;

	};
}

