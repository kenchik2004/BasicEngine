#include "EnemyRandomWalkState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Components/EnemyController.h"

#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

namespace NeonFade
{

	EnemyRandomWalkState::EnemyRandomWalkState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();

		std::function<bool()> default_exit = [this]() {
			// 目的地に近づく or 一定時間経過したら状態遷移を要求する
			static constexpr float DEST_THRESHOLD_SQUARED = DEST_THRESHOLD * DEST_THRESHOLD; // 目的地に近いとみなす距離の二乗
			Vector3 to_destination = next_destination - owner_enemy->transform->position;
			to_destination.y = 0; // Y軸の高さを無視して距離を計算
			return to_destination.magnitudeSquared() < DEST_THRESHOLD_SQUARED || elapsed_time >= MAX_WALK_TIME;
			};
		RegisterChangeRequest("random_walk", default_exit);
	}
	void EnemyRandomWalkState::OnEnter(IStateMachine* machine)
	{
		animator->PlayIfNoSame("enemy_walk", true, Random::Float01());
		next_destination = Random::Position({ -RANDOM_WALK_RADIUS, 0.0f, -RANDOM_WALK_RADIUS }, { RANDOM_WALK_RADIUS, 0.0f, RANDOM_WALK_RADIUS }); // ランダムな目的地を生成
		next_destination += owner_enemy->transform->position; // 現在の位置を基準に目的地を設定

		//next_destination += owner_enemy->enem_controller->GetPlayer()->transform->position; // プレイヤーの位置を基準に目的地を設定

		next_destination.y = 0.0f; // Y軸の高さを固定
		elapsed_time = 0.0f; // 経過時間をリセット

	}
	void EnemyRandomWalkState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;

		Vector3 mov_dir = { 0,0,0 };

		CalculateBaseMovement(mov_dir);

		EnemyRVOSystem::CalculateCohesion(mov_dir, owner_enemy->transform.get(), owner_enemy);
		mov_dir.y = 0;

		Vector3 velocity = rb->velocity;
		static constexpr float ANIMATION_SPEED_ADJUSTMENT_RATIO = 0.09f; // アニメーション速度の調整係数
		// 移動と回転を同時に適用する
		EnemyRVOSystem::ApplyMovementAndRotation(mov_dir, owner_enemy->transform.get(), rb, ROTATION_SPEED, WALK_SPEED);
		//移動速度に応じてアニメーション速度を調整する
		EnemyRVOSystem::AdjustAnimationSpeedByMovementSpeed(mov_dir, animator, velocity, ANIMATION_SPEED_ADJUSTMENT_RATIO);

	}
	void EnemyRandomWalkState::OnExit(IStateMachine* machine)
	{
		rb->velocity = { 0, rb->velocity.y, 0 }; // 状態を抜けるときに速度をリセットする
		animator->anim_speed = 1.0f; // アニメーション速度をリセットする
	}

	bool EnemyRandomWalkState::CanTransitTo(const std::string& state_name)
	{
		//自身への遷移は許可しない
		if (state_name == "random_walk")
			return false;
		return true;
	}

	void EnemyRandomWalkState::DebugDraw()
	{
		Vector3 vel = rb->velocity;
		Vector3 pos = owner_enemy->transform->position;
		DrawLine3D(cast(pos), cast(pos + vel), Color::CYAN);
		DrawSphere3D(cast(next_destination), 1.0f, 16, Color::MAGENTA, Color::MAGENTA, false);
	}

	void EnemyRandomWalkState::CalculateBaseMovement(Vector3& out_mov_dir)
	{
		// 目的地への方向を計算する
		Vector3 to_dest = next_destination - owner_enemy->transform->position;
		to_dest.normalize(); // 目的地へのベクトルを正規化する
		out_mov_dir += to_dest; // 目的地への方向を加算する
	}

}