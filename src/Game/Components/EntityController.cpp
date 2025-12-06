#include "EntityController.h"

namespace NeonFade
{

	int EntityController::Init()
	{
		return Super::Init();
	}
	void EntityController::Update()
	{
		if (current_machine) {
			current_machine->Update(Time::UnscaledDeltaTime());
		}
	}

	void EntityController::SwapMachine(const std::string& name)
	{
		auto it = state_machines.find(name);
		if (it != state_machines.end()) {
			if (current_machine)
				current_machine->OnDeselect();
			current_machine = it->second.get();
			current_machine->OnSelect();
		}
	}

}