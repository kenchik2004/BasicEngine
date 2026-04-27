//---------------------------------------------------------------------------
//! @file   EnemyLeaderStepBackState.cpp
//! @brief  EnemyLeaderStepBackStateの実装。リーダー敵の後退状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyLeaderStepBackState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	EnemyLeaderStepBackState::EnemyLeaderStepBackState(Enemy* owner_, GameObjectWP player_)
		:IState(static_cast<GameObject*>(owner_))
	{
		std::function<bool()> step_back_to_idle = [state = this]() {
			return state->step_back_timer >= state->STEP_BACK_TIME;
			};
		RegisterChangeRequest("idle", step_back_to_idle, 1);
		enemy = owner_;
		rb = enemy->rb.lock().get();
		animator = enemy->animator.lock().get();
		player = player_;
	}
	void EnemyLeaderStepBackState::OnEnter(IStateMachine* machine)
	{
		step_back_timer = 0.0f;
		animator->Play("enemy_stepback", true, 0.0f, 0.05f, false);
		if (player) {
			Vector3 to_player = (player.lock()->transform->position - enemy->transform->position).getNormalized();
			to_player.y = 0.0f;
			if (to_player.magnitudeSquared() > 1e-6f) {
				to_player.normalize();
				enemy->transform->SetAxisZ(to_player);
			}

		}
		jumped = false;
	}
	void EnemyLeaderStepBackState::OnExit(IStateMachine* machine)
	{
	}
	void EnemyLeaderStepBackState::Update(IStateMachine* machine, float dt)
	{
		step_back_timer += dt;
		if (step_back_timer >= 0.5f && !jumped) {
			//後退
			Vector3 back_dir = -enemy->transform->AxisZ();
			back_dir.y = 1.0f;
			rb->AddForce(back_dir * 20.0f, ForceMode::VelocityCange);
			jumped = true;
		}
	}
}