#include "EnemyLeaderApproachToPlayerState.h"

#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

namespace NeonFade
{
	EnemyLeaderApproachToPlayerState::EnemyLeaderApproachToPlayerState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		rb = owner_enemy->rb.lock().get();
		animator = owner_enemy->animator.lock().get();
		player_trns = owner_enemy->enem_controller->GetPlayer().lock()->transform.get();

		// プレイヤーを見失った場合の遷移要求を登録する
		std::function<bool()> lost_exit = [this]() {
			// プレイヤーとの距離が一定以上離れてしばらく経つと状態遷移を要求する
			return player_lost_timer >= PLAYER_LOST_TIME_THRESHOLD;
			};

		// プレイヤーに十分に接近した場合の遷移要求を登録する
		std::function<bool()> approach_exit = [this]() {
			// プレイヤーとの距離が一定以内に近づいた場合に状態遷移を要求する

			// プレイヤーに近づいたとみなす距離の閾値（二乗値）
			static  constexpr float APPROACH_DIST_SQR = PLAYER_APPROACH_DISTANCE_THRESHOLD * PLAYER_APPROACH_DISTANCE_THRESHOLD; 
			return distance_to_player_sqr < APPROACH_DIST_SQR;
			};

		RegisterChangeRequest("idle", lost_exit, 0);
		RegisterChangeRequest("idle", approach_exit, 1);
	}
	void EnemyLeaderApproachToPlayerState::OnEnter(IStateMachine* machine)
	{
		animator->PlayIfNoSame("enemy_fast_run", true);

		Vector3 to_player = player_trns->position - owner_enemy->transform->position;
		to_player.y = 0;

		distance_to_player_sqr = to_player.magnitudeSquared();

		player_lost_timer = 0.0f; // プレイヤーを見失った時間のリセット

		// プレイヤーに近づく際の目標位置のオフセットを設定
		//方向をランダムにする
		goal_offset = Random::Position({ -1.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }); 
		goal_offset = goal_offset.getNormalized() * RANDOM_OFFSET_LENGTH; // 長さを一定にする

	}
	void EnemyLeaderApproachToPlayerState::Update(IStateMachine* machine, float dt)
	{
		if (!rb || !animator || !owner_enemy || !player_trns)
			return;
		Vector3 to_player = player_trns->position - owner_enemy->transform->position;
		// プレイヤーの位置にオフセットを加えることで、他の敵と重ならないようにする
		to_player += goal_offset;
		to_player.y = 0;

		distance_to_player_sqr = to_player.magnitudeSquared();

		EnemyRVOSystem::CalculateCohesion(to_player, owner_enemy->transform.get(), owner_enemy);

		Vector3 velocity = rb->velocity;
		EnemyRVOSystem::ApplyMovementAndRotation(to_player, owner_enemy->transform.get(), rb, ROTATION_SPEED, APPROACH_SPEED);
		EnemyRVOSystem::AdjustAnimationSpeedByMovementSpeed(to_player, animator, velocity, ANIMATION_ADJUSTMENT_RATIO);

		// プレイヤーとの距離が一定以上離れている場合に、プレイヤーを見失った時間を計測する
		// 軽量な比較のために、距離の二乗値を計算する
		static constexpr float PLAYER_LOST_DISTANCE_SQR = PLAYER_LOST_DISTANCE_THRESHOLD * PLAYER_LOST_DISTANCE_THRESHOLD;

		if (distance_to_player_sqr > PLAYER_LOST_DISTANCE_SQR) {
			// プレイヤーを見失った時間を計測する
			player_lost_timer += dt;
		}
		else {
			// プレイヤーを見失っていない場合は、タイマーをリセットする
			player_lost_timer = 0.0f;
		}

	}
	void EnemyLeaderApproachToPlayerState::OnExit(IStateMachine* machine)
	{
		if (rb)
			rb->velocity = { 0, rb->velocity.y, 0 };

		if (animator)
			animator->anim_speed = 1.0f;

	}
	bool EnemyLeaderApproachToPlayerState::CanTransitTo(const std::string& state_name)
	{
		//自分自身への遷移は許可しない
		if (state_name == "approach_player")
			return false;
		return true;
	}
}