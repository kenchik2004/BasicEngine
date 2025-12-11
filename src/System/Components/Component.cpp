#include "System/Objects/Object.h"
#include "Component.h"



void Component::Sleep()
{
	if (!status.status_bit.is(CompStat::STATUS::ACTIVE) && !status.status_bit.is(CompStat::STATUS::DRAW))
		return;
	OnSleep();
	status.status_bit.set(CompStat::STATUS::ACTIVE, false);
	status.status_bit.set(CompStat::STATUS::DRAW, false);
}

void Component::WakeUp()
{
	if (status.status_bit.is(CompStat::STATUS::ACTIVE) && status.status_bit.is(CompStat::STATUS::DRAW))
		return;
	status.status_bit.set(CompStat::STATUS::ACTIVE, true);
	status.status_bit.set(CompStat::STATUS::DRAW, true);
	OnWakeUp();
}

void Component::SetPriority(unsigned int prio)
{
	if (owner)
		owner->SetComponentPriority(prio, shared_from_this());
}


void Component::RemoveThisComponent()
{
	if (owner) {
		owner->RemoveComponent(shared_from_this());
	}
}
