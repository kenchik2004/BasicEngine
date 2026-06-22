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
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCoverApproachState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCoverState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCrowlingState.h"

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
		auto cover_approach_state = make_safe_unique<EnemyCoverApproachState>(state_machine->enemy);
		state_machine->AddState("cover_approach", std::move(cover_approach_state));
		auto cover_state = make_safe_unique<EnemyCoverState>(state_machine->enemy);
		state_machine->AddState("cover", std::move(cover_state));
		auto crowling_state = make_safe_unique<EnemyCrowlingState>(state_machine->enemy);
		state_machine->AddState("crowling", std::move(crowling_state));
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
		//#else
				// ここにAIの思考ロジックを実装する

				//本来なら死亡判定が最優先だが、HPが0になっても殴り続けるのが面白いと思うので、ダメージとノックバックの判定を先にする
		if (is_knock_back) {
			// ノックバックの方向を敵の正面ベクトルとの内積で判定して、前方ノックバックか後方ノックバックかを決定する
			float dot = state_machine->enemy->transform->AxisZ().dot(knock_back_vec);
			if (dot > 0.0f)
				result = "knock_front";
			else
				result = "knock_back";
			ResetFrameParameters();

			return result;
		}
		if (is_damaged) {
			result = "damage";
			ResetFrameParameters();
			return result;
		}

		//プレイヤーの気が済んだら殺してやってもいい
		//HPが0になる or 地面を貫通したら死ぬ
		if (hp == 0 || state_machine->enemy->transform->position.y < -50.0f) {
			result = "die";
			ResetFrameParameters();
			return result;
		}

		if (IsHaveToCoverOtherEnemy()) {
			result = "cover_approach";
			ResetFrameParameters();
			return result;
		}
		if (is_weakened) {
			result = "idle";
			ResetFrameParameters();
			return result;
		}

		if (!IsFoundPlayer()) {
			result = "random_walk";
			ResetFrameParameters();
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
		if (hp <= WEAKED_HP_THRESHOLD) {

			Enemy::RegisterWeakenedEnemy(state_machine->enemy);
			is_weakened = true;
		}
		i_frame_timer = I_FRAME;
		is_damaged = true;
	}
	void BasicEnemyBrain::Die()
	{
		hp = 0;
		is_weakened = false;
	}
	void BasicEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		is_knock_back = true;
		this->knock_back_vec = knock_back_vec;
	}
	void BasicEnemyBrain::DebugDraw()
	{

	}
	void BasicEnemyBrain::ResetFrameParameters()
	{
		is_knock_back = false;
		is_damaged = false;
	}
	bool BasicEnemyBrain::IsFoundPlayer()
	{
		if (!player)
			return false;

		Vector3 to_player = player.lock()->transform->position - state_machine->enemy->transform->position;
		static constexpr float DETECT_RADIUS = 50.0f; // プレイヤーを検知する半径
		static constexpr float DETECT_RADIUS_SQUARED = DETECT_RADIUS * DETECT_RADIUS; // プレイヤーを検知する半径の二乗

		return to_player.magnitudeSquared() < DETECT_RADIUS_SQUARED; // プレイヤーが50ユニット以内にいるかどうか

	}
	bool BasicEnemyBrain::IsHaveToCoverOtherEnemy()
	{
		if (hp <= WEAKED_HP_THRESHOLD)
			return false;

		static constexpr float DETECT_RADIUS = 50.0f; // 弱った仲間を検知する半径
		static constexpr float DETECT_RADIUS_SQUARED = DETECT_RADIUS * DETECT_RADIUS; // 弱った仲間を検知する半径の二乗
		const std::vector<Enemy*>& weakened_enemies = Enemy::GetWeakenedEnemies();
		for (auto& weakened_enemy : weakened_enemies) {
			Vector3 to_weakened_enemy = weakened_enemy->transform->position - state_machine->enemy->transform->position;
			if (to_weakened_enemy.magnitudeSquared() < DETECT_RADIUS_SQUARED) {
				return true;
			}
		}
		return false;
	}
}