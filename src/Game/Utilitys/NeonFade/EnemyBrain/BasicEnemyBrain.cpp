//---------------------------------------------------------------------------
//! @file   BasicEnemyBrain.cpp
//! @brief  BasicEnemyBrainの実装。基本的な敵AI思考ロジックを実装する
//---------------------------------------------------------------------------
#include "BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"

#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyIdleState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyKnockBackState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyKnockFrontState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDamageState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyDieState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyStandUpFrontState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyStandUpBackState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyRandomWalkState.h"

namespace NeonFade {
	BasicEnemyBrain::BasicEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		:AbstractEnemyBrain(state_machine_, player_)
	{
		auto idle_state = make_safe_unique<EnemyIdleState>(state_machine->enemy);
		state_machine->AddState("idle", std::move(idle_state));
		auto damage_state = make_safe_unique<EnemyDamageState>(state_machine->enemy);
		state_machine->AddState("damage", std::move(damage_state));
		auto knock_back_state = make_safe_unique<EnemyKnockBackState>(state_machine->enemy);
		state_machine->AddState("knock_back", std::move(knock_back_state));
		auto knock_front_state = make_safe_unique<EnemyKnockFrontState>(state_machine->enemy);
		state_machine->AddState("knock_front", std::move(knock_front_state));
		auto die_state = make_safe_unique<EnemyDieState>(state_machine->enemy);
		state_machine->AddState("die", std::move(die_state));
		auto stand_up_front_state = make_safe_unique<EnemyStandUpFrontState>(state_machine->enemy);
		state_machine->AddState("stand_up_front", std::move(stand_up_front_state));
		auto stand_up_back_state = make_safe_unique<EnemyStandUpBackState>(state_machine->enemy);
		state_machine->AddState("stand_up_back", std::move(stand_up_back_state));
		auto random_walk_state = make_safe_unique<EnemyRandomWalkState>(state_machine->enemy);
		state_machine->AddState("random_walk", std::move(random_walk_state));
		state_machine->ChangeState("idle");


	}
	std::string BasicEnemyBrain::Think()
	{
		i_frame_timer -= Time::DeltaTime();

		std::string result = "";
#ifndef NDEBUG
		if (Input::GetKeyDown(KeyCode::Alpha1))
			result = "idle";
		if (Input::GetKeyDown(KeyCode::Alpha2))
			result = "damage";
		if (Input::GetKeyDown(KeyCode::Alpha3))
			result = "knock_back";
		if (Input::GetKeyDown(KeyCode::Alpha4))
			result = "knock_front";
		if (Input::GetKeyDown(KeyCode::Alpha5))
			result = "die";

		// ここにAIの思考ロジックを実装する

		//本来なら死亡判定が最優先だが、HPが0になっても殴り続けるのが面白いと思うので、ダメージとノックバックの判定を先にする
		if (is_knock_back) {
			// ノックバックの方向を敵の正面ベクトルとの内積で判定して、前方ノックバックか後方ノックバックかを決定する
			float dot = state_machine->enemy->transform->AxisZ().dot(knock_back_vec);
			if (dot > 0.0f)
				result = "knock_front";
			else
				result = "knock_back";
			ResetParameters();

			return result;
		}
		if (is_damaged) {
			result = "damage";
			ResetParameters();
			return result;
		}

		//プレイヤーの気が済んだら殺してやってもいい
		//HPが0になる or 地面を貫通したら死ぬ
		if (hp == 0 || state_machine->enemy->transform->position.y < -50.0f) {
			result = "die";
			ResetParameters();
			return result;
		}

		if (!IsFoundPlayer()) {
			result = "random_walk";
			ResetParameters();
			return result;
		}

#endif
			return result;
	}
	void BasicEnemyBrain::Damage(u32 damage, bool ignore_i_frame) {
		if (!ignore_i_frame && i_frame_timer > 0.0f)
			return;
		if (damage > hp)
		{
			Die();
			return;
		}
		hp -= damage;
		i_frame_timer = I_FRAME;
		is_damaged = true;
	}
	void BasicEnemyBrain::Die()
	{
		hp = 0;
	}
	void BasicEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		is_knock_back = true;
		this->knock_back_vec = knock_back_vec;
	}
	void BasicEnemyBrain::DebugDraw()
	{

	}
	void BasicEnemyBrain::ResetParameters()
	{
		is_knock_back = false;
		is_damaged = false;
	}
	bool BasicEnemyBrain::IsFoundPlayer()
	{
		if (!player)
			return false;

		Vector3 to_player = player.lock()->transform->position - state_machine->enemy->transform->position;
		return to_player.magnitudeSquared() < 50.0f * 50.0f; // プレイヤーが50ユニット以内にいるかどうか

	}
}