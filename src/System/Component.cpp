#include "precompile.h"
#include "Component.h"


void Component::SetPriority(int prio)
{
	status.priority = prio;
	owner->ChangedCompPriority(true);
}

void Component::RemoveThisComponent()
{
	owner->RemoveComponent(shared_from_this());
}
