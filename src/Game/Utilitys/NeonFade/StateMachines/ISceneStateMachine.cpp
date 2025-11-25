#include "ISceneStateMachine.h"


namespace NeonFade {
	ISceneStateMachine::ISceneStateMachine(Scene* owner_scene_)
	{
		owner_scene = owner_scene_;
	}
	void ISceneStateMachine::ChangeState(std::string_view next)
	{
		auto it = states.find(next.data());
		if (it != states.end())
		{
			if (current_state)
			{
				current_state->OnExit(this);
			}
			current_state = it->second.get();
			if (current_state)
			{
				current_state->OnEnter(this);
			}
		}
	}

	void ISceneStateMachine::Update(float dt)
	{
		if (current_state)
		{
			current_state->Update(this, dt);
			auto&& request_state = current_state->GetChangeRequest();
			if (!request_state.empty())
			{
				ChangeState(request_state);
			}
		}
	}

	void ISceneStateMachine::AddState(std::string_view name, SafeUniquePtr<ISceneState> state) {
		state->SetName(name);
		states[name.data()] = std::move(state);
	}
}