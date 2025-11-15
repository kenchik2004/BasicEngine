#include "IStateMachine.h"
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	IStateMachine::IStateMachine(GameObject* owner_)
	{
		owner = owner_;
	}
	void IStateMachine::ChangeState(std::string_view next)
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

	void IStateMachine::Update(float dt)
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

	void IStateMachine::AddState(std::string_view name, SafeUniquePtr<IState> state) {
		state->SetName(name);
		states[name.data()] = std::move(state);
	}
	void IStateMachine::DebugDraw()
	{
		if (current_state)
		{
			printfDx("%s Current State: %s\n", owner->name.c_str(), current_state->GetName().c_str());
		}
	}

	void IStateMachine::OnCollisionEnter(const HitInfo& hit_info)
	{
		if (!current_state) return;
		current_state->OnCollisionEnter(this, hit_info);
	}
	void IStateMachine::OnCollisionStay(const HitInfo& hit_info)
	{
		if (!current_state) return;
		current_state->OnCollisionStay(this, hit_info);
	}
	void IStateMachine::OnCollisionExit(const HitInfo& hit_info)
	{
		if (!current_state) return;
		current_state->OnCollisionExit(this, hit_info);
	}
	void IStateMachine::OnTriggerEnter(const HitInfo& hit_info)
	{
		if (!current_state) return;
		current_state->OnTriggerEnter(this, hit_info);
	}
	void IStateMachine::OnTriggerStay(const HitInfo& hit_info)
	{
		if (!current_state) return;
		current_state->OnTriggerStay(this, hit_info);
	}
	void IStateMachine::OnTriggerExit(const HitInfo& hit_info)
	{
		if (!current_state) return;
		current_state->OnTriggerExit(this, hit_info);
	}
}