#include "EnemyRandomWalkState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Components/EnemyController.h"

#include "Game/Utilitys/NeonFade/EnemyBrain/EnemyRVOSystem.h"

namespace NeonFade
{
	//--------------------------------------------------------------------------------------------------------------
	// @brief アニメーション速度を移動速度に応じて調整する関数
	// 移動速度とアニメーション速度が一致していないと、足の滑り(ムーンウォークのような気持ち悪さ)が発生するので、
	// 再生速度の調整を行う。
	// 物理的に1m歩く間にアニメーションデータ内で何m歩くかを調整するための係数を adjustment_ratio として渡す
	// @param forward: 敵の前方向ベクトル
	// @param anim: Animatorコンポーネントへのポインタ
	// @param rb: RigidBodyコンポーネントへのポインタ
	// @param adjustment_ratio: 物理的な1mの移動に対して、アニメーションデータ内で何m歩くかを調整する係数
	//--------------------------------------------------------------------------------------------------------------
	void AdjustAnimationSpeedByMovementSpeed(Vector3 forward, Animator* anim, RigidBody* rb, float adjustment_ratio)
	{
		if (!anim || !rb) return;
		Vector3 velocity = rb->velocity;
		velocity.y = 0; // Y軸の速度を無視する
		//動こうとしている方向に対し、実際どれくらい進めているのかを計算する
		float current_speed = forward.dot(velocity);
		float speed_ratio = current_speed * adjustment_ratio;
		anim->anim_speed = max(0.01f, speed_ratio);
	}
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
		auto another_enemies_ = SceneManager::Object::GetArray<Enemy>(); // 同じシーン内の他の敵を取得

	}
	void EnemyRandomWalkState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;

		Vector3 mov_dir = { 0,0,0 };

		CalculateBaseMovement(mov_dir);

		EnemyRVOSystem::CalculateCohesion(mov_dir, owner_enemy->transform.get(), owner_enemy);
		mov_dir.y = 0;
		//移動速度に応じてアニメーション速度を調整する
		//物理的に1m歩くと、アニメーションを0.09倍の速度で再生するように調整すると、足の滑りがなくなる
		static constexpr float ANIMATION_SPEED_ADJUSTMENT_RATIO = 0.09f; // アニメーション速度の調整係数
		AdjustAnimationSpeedByMovementSpeed(owner_enemy->transform->AxisZ(), animator, rb, ANIMATION_SPEED_ADJUSTMENT_RATIO);
		EnemyRVOSystem::ApplyMovementAndRotation(mov_dir, owner_enemy->transform.get(), rb, ROTATION_SPEED, WALK_SPEED);

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