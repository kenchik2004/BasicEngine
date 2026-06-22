//---------------------------------------------------------------------------
//! @file   Component.cpp
//! @brief  コンポーネント基底クラスの実装（Sleep/WakeUp/優先度設定など）
//---------------------------------------------------------------------------
#include "System/Objects/Object.h"
#include "Component.h"



// コンポーネントを非アクティブ化する（既に非アクティブなら何もしない）
void Component::Sleep(bool sleep_draw)
{
	if (!status.status_bit.is(CompStat::STATUS::ACTIVE) && !status.status_bit.is(CompStat::STATUS::DRAW))
		return;
	OnSleep();
	status.status_bit.set(CompStat::STATUS::ACTIVE, false);

	if (sleep_draw)
		status.status_bit.set(CompStat::STATUS::DRAW, false);
}

// コンポーネントをアクティブ化する（既にアクティブなら何もしない）
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
