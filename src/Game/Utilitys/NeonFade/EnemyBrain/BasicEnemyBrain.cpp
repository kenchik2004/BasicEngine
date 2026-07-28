//---------------------------------------------------------------------------
//! @file   BasicEnemyBrain.cpp
//! @brief  BasicEnemyBrainの実装。基本的な敵AI思考ロジックを実装する
//---------------------------------------------------------------------------
#include "BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"

#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyRandomWalkState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCoverApproachState.h"
#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyCoverState.h"

namespace NeonFade {



	BasicEnemyBrain::BasicEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		:AbstractEnemyBrain(state_machine_, player_)
	{

		// 各ステートをステートマシンに登録する

		//基礎的なステートは親クラスで登録するので、ここでは追加のステートのみを登録する


		//徘徊状態
		auto random_walk_state = make_safe_unique<EnemyRandomWalkState>(state_machine->enemy);
		state_machine->AddState("random_walk", std::move(random_walk_state));
		//仲間を庇いに行く状態
		auto cover_approach_state = make_safe_unique<EnemyCoverApproachState>(state_machine->enemy);
		state_machine->AddState("cover_approach", std::move(cover_approach_state));
		//仲間を庇う状態
		auto cover_state = make_safe_unique<EnemyCoverState>(state_machine->enemy);
		state_machine->AddState("cover", std::move(cover_state));

		// 初期状態を待機状態に設定する
		state_machine->ChangeState("idle");


	}
	std::string BasicEnemyBrain::Think()
	{
		i_frame_timer -= Time::DeltaTime();

		std::string result = __super::Think();

		// もし親クラスのThinkで遷移先が決まっていれば、その遷移先を返す
		if (result != "")
			return result;


		// 弱った仲間を援護する必要がある場合は援護状態に遷移する
		if (IsHaveToCoverOtherEnemy()) {
			// 仲間を援護しに行く
			// このステートからは、自動的に庇う状態に遷移するので、ここでは入口として援護しに行く状態に遷移する
			result = "cover_approach";

			// 援護状態に遷移したら、フレーム依存パラメータをリセットする
			ResetFrameParameters();
			return result;
		}


		// プレイヤーを発見していない場合は徘徊状態に遷移する
		if (!IsFoundPlayer()) {
			result = "random_walk";

			// 徘徊状態に遷移したら、フレーム依存パラメータをリセットする
			ResetFrameParameters();
			return result;
		}

		return result;
	}

	bool BasicEnemyBrain::IsFoundPlayer()
	{
		if (!player)
			return false;

		Vector3 to_player = player.lock()->transform->position - state_machine->enemy->transform->position;
		static constexpr float DETECT_RADIUS = 50.0f; // プレイヤーを検知する半径
		static constexpr float DETECT_RADIUS_SQUARED = DETECT_RADIUS * DETECT_RADIUS; // プレイヤーを検知する半径の二乗

		return to_player.magnitudeSquared() < DETECT_RADIUS_SQUARED; // プレイヤーが50m以内にいるかどうか

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