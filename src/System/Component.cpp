#include "precompile.h"
#include "Component.h"


void Component::SetPriority(int prio)
{
	status.priority = prio;
	owner.lock()->ChangedCompPriority(true);
}

void Component::RemoveThisComponent()
{
	owner.lock()->RemoveComponent(shared_from_this());
}
