#include "EnemyCrowlingState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/EnemyController.h"

namespace NeonFade {
	EnemyCrowlingState::EnemyCrowlingState(Enemy* owner_enemy_)
		:IState(static_cast<GameObject*>(owner_enemy_))
	{
		owner_enemy = owner_enemy_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
		col = owner_enemy->col.lock().get();
		std::function<bool()> default_exit = [this]() {
			const Vector3& my_pos = owner_enemy->transform->position;
			const Vector3& player_pos = owner_enemy->enem_controller->GetPlayer()->transform->position;
			float distance_squared = (player_pos - my_pos).magnitudeSquared();
			// プレイヤーとの距離が一定以上離れていれば這いずり状態を抜ける
			static constexpr float CONTINUE_RANGE_THRESHOLD_SQUARED = CROWLING_CONTINUE_RANGE_THRESHOLD * CROWLING_CONTINUE_RANGE_THRESHOLD;

			return elapsed_time >= CROWLING_DURATION && distance_squared >= CONTINUE_RANGE_THRESHOLD_SQUARED;
			};
		RegisterChangeRequest("stand_up_front", default_exit);

		std::function<void()> apply_movement0 = [this]() {
			ApplyMovement(movement_direction);
			};

		//! アニメーションコールバックをセットするとき、中でstd::moveが走っているので
		//! std::functionを直接渡すと、1回目のコールバックが呼ばれた後にstd::functionの中身が空になってしまい、2回目のコールバックが呼ばれなくなってしまう
		//! 先に内容が同じstd::functionを2つ用意して、それぞれをコールバックに渡すことで、両方のコールバックが正しく呼ばれるようにする
		std::function<void()> apply_movement1 = [this]() {
			ApplyMovement(movement_direction);
			};

		// 這いずりアニメーションの特定のフレームでApplyMovement関数を呼び出すコールバックを登録する
		animator->SetAnimationCallBack("enemy_crowling", apply_movement0, APPLY_MOVEMENT_FRAME_INDICES[0], "apply_movement0");
		animator->SetAnimationCallBack("enemy_crowling", apply_movement1, APPLY_MOVEMENT_FRAME_INDICES[1], "apply_movement1");

	}
	void EnemyCrowlingState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("enemy_crowling", true, 0.0f, 0.5f);
		// 這いずり状態の物理マテリアルを設定する（摩擦係数の低いものにして、滑りやすくする）
		col->SetMaterial(PhysicMaterial::Default);
	}
	void EnemyCrowlingState::OnExit(IStateMachine* machine)
	{
		rb->velocity = { 0, rb->velocity.y, 0 }; // 状態を抜けるときに速度をリセットする

		// 這いずり状態の物理マテリアルをもとに戻す
		col->SetMaterial(PhysicMaterial::HighFriction);
	}
	void EnemyCrowlingState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;

		const Vector3& my_pos = owner_enemy->transform->position;
		const Vector3& player_pos = owner_enemy->enem_controller->GetPlayer()->transform->position;
		movement_direction = (my_pos - player_pos).getNormalized(); // プレイヤーと逆方向を向くように移動方向を設定する


		// ApplyMovementはアニメーションコールバックで呼び出されるため、ここでは呼び出さない
		ApplyRotation(movement_direction); // 移動方向に基づいて回転を適用する

	}
	bool EnemyCrowlingState::CanTransitTo(const std::string& state_name)
	{
		if (state_name == "knock_back" || state_name == "knock_front")
			return true;
		return false;
	}
	void EnemyCrowlingState::ApplyMovement(Vector3& mov_dir)
	{
		mov_dir.y = 0; // 水平方向の移動に限定する
		mov_dir = mov_dir.getNormalized() * CROWLING_SPEED; // 移動方向を正規化してから這いずり速度を掛ける
		mov_dir.y = rb->velocity.y; // 現在の垂直速度を保持する
		rb->velocity = mov_dir; // 計算した移動ベクトルを剛体の速度に直接設定する	
	}
	void EnemyCrowlingState::ApplyRotation(const Vector3& mov_dir)
	{
		if (mov_dir.magnitudeSquared() > 0.001f) { // 移動方向がほとんどゼロでない場合にのみ回転を適用する
			Vector3 target_dir = mov_dir; // 目標の回転方向は移動方向と同じ
			const Vector3 forward = owner_enemy->transform->AxisZ(); // 敵の前方ベクトルを取得する
			// 現在の前方ベクトルと移動方向から、目標の回転を計算する
			target_dir.y = 0; // 水平方向の回転に限定する
			target_dir.normalize(); // 目標方向を正規化する
			target_dir = Slerp(forward, target_dir, CROWLING_ROTATION_SPEED); // 線形補間で回転を滑らかにする
			owner_enemy->transform->SetAxisZ(target_dir); // 敵の前方ベクトルを目標の回転方向に設定する
		}
	}
}