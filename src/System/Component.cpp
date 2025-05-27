#include "precompile.h"
#include "System/ObjBase.h"
#include "Component.h"



void Component::SetPriority(int prio)
{
	owner->SetComponentPriority(prio, shared_from_this());
}


void Component::RemoveThisComponent()
{
	owner.lock()->RemoveComponent(shared_from_this());
}
