#include "precompile.h"
#include "EnemyController.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"

namespace NeonFade {


	int EnemyController::Init()
	{
		SafeSharedPtr<Enemy> owner_enemy = SafeStaticCast<Enemy>(owner.lock());
		state_machine = make_safe_unique<EnemyStateMachine>(owner_enemy.get());

		return 0;
	}

	void EnemyController::Update()
	{
		brain->Think();
		state_machine->Update(Time::UnscaledDeltaTime());
	}

	void EnemyController::Exit()
	{
	}

	void EnemyController::LateDebugDraw()
	{
		state_machine->DebugDraw();
		brain->DebugDraw();
	}

	void EnemyController::Damage(int damage, bool ignore_i_frame)
	{
		brain->Damage(damage, ignore_i_frame);
	}

	void EnemyController::Down(Vector3 vec)
	{
		brain->KnockBack(vec);
	}
	bool EnemyController::IsDead() {
		if (!brain)
			return true;
		return brain->IsDead();
	}
	u32 EnemyController::GetHp() {
		if (!brain)
			return 0;
		return brain->GetHp();
	}

	void EnemyController::OnTriggerEnter(const HitInfo& hit_info)
	{
		state_machine->OnTriggerEnter(hit_info);
	}

}
