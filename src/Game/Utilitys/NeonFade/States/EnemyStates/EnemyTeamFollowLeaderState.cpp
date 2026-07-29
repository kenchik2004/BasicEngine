#include "EnemyTeamFollowLeaderState.h"
#include "Game/Objects/NeonFade/Enemy.h"

#include "Game/Components/EnemyController.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyTeam.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

namespace NeonFade
{
	EnemyTeamFollowLeaderState::EnemyTeamFollowLeaderState(Enemy* owner_enemy_)
		: IState(owner_enemy_)
	{
		// 所有者のEnemyオブジェクトへのポインタを保持する
		owner_enemy = owner_enemy_;

		//依存するコンポーネントのポインタを取得する
		rb = owner_enemy->rb.lock().get();
		animator = owner_enemy->animator.lock().get();

	}
	void EnemyTeamFollowLeaderState::Update(IStateMachine* machine, float dt)
	{
		if (!rb || !animator || !owner_enemy)
			return;

		// 所有者のEnemyオブジェクトの脳を取得する
		TeamMemberEnemyBrain* brain = dynamic_cast<TeamMemberEnemyBrain*>(owner_enemy->enem_controller->GetBrain());

		//脳が存在しないorチームメンバー専用脳でない場合は処理を中断する
		if (!brain)
			return;

		EnemyTeam* team = brain->GetTeam();
		// チームが存在しない場合orリーダーが存在しない場合は処理を中断する
		if (!team || !team->GetLeader())
			return;

		// リーダーの位置を取得する
		Enemy* leader_obj = team->GetLeader()->GetOwnerBody();
		Vector3 leader_pos = leader_obj->transform->position;
		Vector3 to_leader = leader_pos - owner_enemy->transform->position;

		// リーダーまでの距離の二乗を保持する
		distance_to_leader_sqr = to_leader.magnitudeSquared();

		// リーダーの方向に向かって移動する
		//他の敵との衝突を避けるために、RVOシステムを使用して移動方向を計算する
		EnemyRVOSystem::CalculateCohesion(to_leader, owner_enemy->transform.get(), owner_enemy);
		// Y軸の高さを無視して移動方向を計算する
		to_leader.y = 0;


		Vector3  velocity = rb->velocity;
		// 移動と回転を同時に適用する
		EnemyRVOSystem::ApplyMovementAndRotation(to_leader, owner_enemy->transform.get(), rb, rotation_speed, follow_speed);

		// 移動方向に応じてアニメーション速度を調整する
		EnemyRVOSystem::AdjustAnimationSpeedByMovementSpeed(to_leader, animator, velocity, animation_adjustment_ratio);

	}

	void EnemyTeamFollowLeaderState::OnExit(IStateMachine* machine)
	{
		if (animator)
			animator->anim_speed = 1.0f; // アニメーション速度をリセットする
	}


	EnemyTeamFollowLeaderWalkState::EnemyTeamFollowLeaderWalkState(Enemy* owner_enemy_)
		:EnemyTeamFollowLeaderState(owner_enemy_)
	{
		//親クラスで使う変数を上書き
		follow_speed = WALK_SPEED;
		rotation_speed = ROTATION_SPEED;
		animation_adjustment_ratio = ANIMATION_ADJUSTMENT_RATIO;


		std::function<bool()> to_run_state = [this]() {
			// リーダーとの距離が一定以上になったら走るフォローステートに遷移する
			static constexpr float TO_RUN_THRESHOLD_SQR = TO_RUN_DISTANCE_THRESHOLD * TO_RUN_DISTANCE_THRESHOLD;
			return distance_to_leader_sqr > TO_RUN_THRESHOLD_SQR;
			};

		RegisterChangeRequest("follow_leader_run", to_run_state);

		std::function <bool()> to_idle_state = [this]() {
			// リーダーとの距離が一定以下になったら追従を終了して待機ステートに遷移する
			static constexpr float FINISH_THRESHOLD_SQR = FINISH_DISTANCE_THRESHOLD * FINISH_DISTANCE_THRESHOLD;
			return distance_to_leader_sqr < FINISH_THRESHOLD_SQR;
			};
		RegisterChangeRequest("idle", to_idle_state);

	}


	void EnemyTeamFollowLeaderWalkState::OnEnter(IStateMachine* machine)
	{
		if (animator)
			animator->PlayIfNoSame("enemy_walk", true, 0.0f, 0.1f);
	}


	bool EnemyTeamFollowLeaderWalkState::CanTransitTo(const std::string& state_name)
	{
		//走るフォローステートへは、自動で遷移を行うため、外部からの遷移要求は許可しない
		//また、自身への遷移も許可しない
		if (state_name == "follow_leader_run" || state_name == "follow_leader")
			return false;
		return true;
	}

	void EnemyTeamFollowLeaderWalkState::DebugDraw()
	{
		DrawSphere3D(cast(owner_enemy->transform->position + Vector3(0, 5, 0)), 3.0f, 8, 0xffff00, 0xffff00, false);
	}
	
	EnemyTeamFollowLeaderRunState::EnemyTeamFollowLeaderRunState(Enemy* owner_enemy_)
		:EnemyTeamFollowLeaderState(owner_enemy_)
	{

		//親クラスで使う変数を上書き
		follow_speed = FOLLOW_LEADER_RUN_SPEED;
		rotation_speed = FOLLOW_LEADER_RUN_ROTATION_SPEED;
		animation_adjustment_ratio = RUN_ANIMATION_ADJUSTMENT_RATIO;

		std::function<bool()> to_walk_state = [this]() {
			// リーダーとの距離が一定以下になったら通常のフォローステートに遷移する
			static constexpr float FOLLOW_RUN_THRESHOLD_SQR = FOLLOW_LEADER_RUN_DISTANCE_THRESHOLD * FOLLOW_LEADER_RUN_DISTANCE_THRESHOLD;
			return distance_to_leader_sqr < FOLLOW_RUN_THRESHOLD_SQR;
			};

		RegisterChangeRequest("follow_leader", to_walk_state);

	}

	void EnemyTeamFollowLeaderRunState::OnEnter(IStateMachine* machine)
	{
		//アニメーションのみ変更
		if (animator)
			animator->PlayIfNoSame("enemy_fast_run", true);
	}

	bool EnemyTeamFollowLeaderRunState::CanTransitTo(const std::string& state_name)
	{
		//通常のフォローステートへは、自動で遷移を行うため、外部からの遷移要求は許可しない
		//また、自身への遷移も許可しない
		if (state_name == "follow_leader" || state_name == "follow_leader_run")
			return false;
		return	true;
	}

	void EnemyTeamFollowLeaderRunState::DebugDraw()
	{
		printfDx("anim_speed: %f\n", animator->anim_speed);
		DrawSphere3D(cast(owner_enemy->transform->position + Vector3(0, 5, 0)), 3.0f, 8, 0xffffff, 0xffffff, false);
	}

}