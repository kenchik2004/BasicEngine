//---------------------------------------------------------------------------
//! @file   EnemyController.cpp
//! @brief  EnemyControllerコンポーネントの実装。敵キャラクターの行動制御を管理する
//---------------------------------------------------------------------------
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

		return Super::Init();
	}

	void EnemyController::Update()
	{
		if (!brain || !state_machine)
			return;

		std::string best_action = brain->Think();
		IState* current_state = state_machine->GetCurrentState();
		if (!current_state || current_state->CanTransitTo(best_action)) {
			state_machine->ChangeState(best_action);
		}

		state_machine->Update(Time::UnscaledDeltaTime());
	}

	void EnemyController::Exit()
	{}

	void EnemyController::LateDebugDraw()
	{
		if (!brain || !state_machine)
			return;

		state_machine->DebugDraw();
		brain->DebugDraw();
	}

	void EnemyController::Damage(int damage, bool ignore_i_frame)
	{
		if (!brain)
			return;

		brain->Damage(damage, ignore_i_frame);
	}

	void EnemyController::Down(Vector3 vec)
	{
		if (!brain)
			return;
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

	//! @brief 脳（AI）をセットする
	void EnemyController::SetBrain(AbstractEnemyBrainUP new_brain) {
		brain = std::move(new_brain); brain->Think();
		if (state_machine)
			state_machine->SetBrain(brain.get());
	}

	void EnemyController::OnTriggerEnter(const HitInfo& hit_info)
	{
		if (!state_machine)
			return;
		state_machine->OnTriggerEnter(hit_info);
	}

}
