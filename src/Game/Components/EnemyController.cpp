#include "precompile.h"
#include "EnemyController.h"
#include "Game/Objects/NeonFade/Enemy.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/States/EnemyIdleState.h"
#include "Game/Utilitys/NeonFade/States/EnemyDamageState.h"
#include "Game/Utilitys/NeonFade/States/EnemyDownState.h"

int NeonFade::EnemyController::Init()
{
	SafeSharedPtr<Enemy> owner_enemy = SafeStaticCast<Enemy>(owner.lock());
	state_machine = make_safe_unique<EnemyStateMachine>(owner_enemy.get());
	state_machine->AddState("idle", make_safe_unique<EnemyIdleState>(owner_enemy.get()));
	state_machine->AddState("damage", make_safe_unique<EnemyDamageState>(owner_enemy.get()));
	state_machine->AddState("down", make_safe_unique<EnemyDownState>(owner_enemy.get()));
	state_machine->ChangeState("idle");
	return 0;
}

void NeonFade::EnemyController::Update()
{
	state_machine->Update(Time::DeltaTime());
}

void NeonFade::EnemyController::Exit()
{
}

void NeonFade::EnemyController::LateDebugDraw()
{

}

void NeonFade::EnemyController::Damage()
{
	if (state_machine->GetCurrentStateName() == "damage")
		state_machine->ChangeState("idle");
	state_machine->ChangeState("damage");
}

void NeonFade::EnemyController::Down(Vector3 vec)
{
	owner->transform->SetAxisZ(vec);
	state_machine->ChangeState("down");
}
