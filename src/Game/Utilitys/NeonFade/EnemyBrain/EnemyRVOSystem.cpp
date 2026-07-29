#include "EnemyRVOSystem.h"

namespace NeonFade {
	//! @brief 近くの敵に引き寄せられる方向を計算する関数
	//! @param out_mov_dir 計算結果の移動方向を格納するVector3参照
	//! @param transform 自身のTransformポインタ
	//! @param owner_enemy 自身のEnemyポインタ
	//! @param cohesion_weight 重力の強さ
	void EnemyRVOSystem::CalculateCohesion(Vector3& out_mov_dir, const Transform* transform, const Enemy* owner_enemy, float cohesion_weight)
	{

		Vector3 cohesion = Vector3(0, 0, 0);
		// 近くの敵に少し引き寄せられるようにする
		const std::vector<Enemy*>& all_enemies = Enemy::GetAllEnemies();
		const u32 num_enemies = static_cast<u32>(all_enemies.size());
		Vector3 owner_pos = transform->position;
		for (u32 i = 0; i < num_enemies; ++i) {
			Enemy* enemy = all_enemies[i];
			//自身の情報は無視する
			if (enemy != owner_enemy) {
				Vector3 to_enemy = enemy->transform->position - owner_pos;
				float distance_squared = to_enemy.magnitudeSquared();
				distance_squared = max(distance_squared, 0.0001f); // ゼロ除算を防ぐために最小値を設定

				static constexpr float APPROACH_DISTANCE_MAX = 50.0f; // 影響を与える最大距離
				static constexpr float APPROACH_DISTANCE_MIN = 15.0f; // 影響を与える最小距離
				static constexpr float LEAVE_DISTANCE_MIN = 10.0f; // あまりにも近い場合に離れる距離
				static constexpr float APMAX_SQRD = APPROACH_DISTANCE_MAX * APPROACH_DISTANCE_MAX; // 影響を与える最大距離の二乗
				static constexpr float APMIN_SQRD = APPROACH_DISTANCE_MIN * APPROACH_DISTANCE_MIN; // 影響を与える最小距離の二乗
				static constexpr float LEAVE_SQRD = LEAVE_DISTANCE_MIN * LEAVE_DISTANCE_MIN; // あまりにも近い場合に離れる距離の二乗
				static constexpr float APPROACH_FACTOR = 0.02f; // 近くの敵に引き寄せられる係数
				static constexpr float LEAVE_FACTOR = 2.5f; // あまりにも近い場合に離れる係数

				if (distance_squared < APMAX_SQRD && distance_squared > APMIN_SQRD) { // 近くの敵に対してのみ影響を与える
					float sqrt_dist = sqrtf(distance_squared);
					float x = (sqrt_dist - APPROACH_DISTANCE_MIN) / (APPROACH_DISTANCE_MAX - APPROACH_DISTANCE_MIN); // 影響の強さを距離に応じて変化させる
					float smoothstep = x * x * (3 - 2 * x); // 影響の強さを距離に応じて変化させる

					cohesion += to_enemy.getNormalized() * smoothstep * APPROACH_FACTOR; // 引き寄せる方向に力を加える
				}
				if (distance_squared < LEAVE_SQRD) { // あまりにも近い場合は少し離れるようにする
					float sqrt_dist = sqrtf(distance_squared);
					float x = (LEAVE_DISTANCE_MIN - sqrt_dist) / LEAVE_DISTANCE_MIN; // 影響の強さを距離に応じて変化させる
					float smoothstep = x * x * (3 - 2 * x); // 影響の強さを距離に応じて変化させる

					cohesion -= to_enemy.getNormalized() * smoothstep * LEAVE_FACTOR; // 離れる方向に力を加える
				}
			}
		}

		float dot = out_mov_dir.dot(cohesion);
		float direction_factor = (dot >= 0) ? 1.0f : 0.5f; // 引き寄せる方向と同じならそのまま、逆なら半分の力にする
		out_mov_dir += cohesion * direction_factor * cohesion_weight; // 引き寄せる力を加える
	}

	void EnemyRVOSystem::SmoothMovement(Vector3& mov_dir, const Vector3& forward, float smoothing_factor, float dt)
	{
		// Slerpを使って回転を滑らかにする
		mov_dir = Slerp(forward, mov_dir, smoothing_factor * dt);
	}

	//! @brief 最終的な移動方向を正規化し、速度を適用する関数
	//! @param mov_dir 計算された移動方向を格納するVector3参照
	//! @param rb 自身のRigidBodyポインタ
	//! @param movement_factor 移動速度の係数
	void EnemyRVOSystem::ApplyMovement(Vector3& mov_dir, RigidBody* rb, const float movement_factor)
	{
		mov_dir.y = 0; // Y軸の速度を無効にする
		mov_dir.normalize();
		mov_dir *= movement_factor; // 移動速度を調整
		mov_dir.y = rb->velocity.y; // 現在のY軸の速度を保持
		rb->velocity = mov_dir;
	}


	//! @brief 最終的な移動方向を正規化し、回転を適用する関数
	//! @param mov_dir 計算された移動方向を格納するVector3参照
	//! @param transform 自身のTransformポインタ
	//! @param rotation_factor 回転速度の係数
	void EnemyRVOSystem::ApplyRotation(Vector3& mov_dir, Transform* transform, const float rotation_factor)
	{
		// 移動方向がほとんどゼロでない場合にのみ回転を適用する
		if (mov_dir.magnitudeSquared() <= 0.001f)
			return;


		mov_dir.y = 0; // Y軸の回転を無効にする
		mov_dir.normalize();
		Vector3 forward = transform->AxisZ();
		mov_dir = Slerp(forward, mov_dir, rotation_factor * Time::DeltaTime());
		transform->SetAxisZ(mov_dir); // 目的地への方向を向く
	}

	//! @brief 最終的な移動方向を正規化し、速度と回転を適用する関数
	//! @param mov_dir 計算された移動方向を格納するVector3参照
	//! @param transform 自身のTransformポインタ
	//! @param rb 自身のRigidBodyポインタ
	//! @param rotation_factor 回転速度の係数
	//! @param movement_factor 移動速度の係数
	void EnemyRVOSystem::ApplyMovementAndRotation(Vector3& mov_dir, Transform* transform, RigidBody* rb, const float rotation_factor, const float movement_factor)
	{
		mov_dir.y = 0; // Y軸の回転を無効にする

		// 移動方向を正規化して回転を適用
		//ここでApplyRotationとApplyMovementを別々に呼び出すと、正規化が2回走ってしまうのでコストがかかる
		//まとめて処理し、Y軸のみを編集することで正規化を1回に抑える
		mov_dir.normalize();
		// Slerpを使って回転を滑らかにする
		SmoothMovement(mov_dir, transform->AxisZ(), rotation_factor, Time::DeltaTime());

		transform->SetAxisZ(mov_dir); // 目的地への方向を向く

		mov_dir *= movement_factor;
		mov_dir.y = rb->velocity.y; // 現在のY軸の速度を保持
		rb->velocity = mov_dir;
	}


	//--------------------------------------------------------------------------------------------------------------
	// @brief アニメーション速度を移動速度に応じて調整する関数
	// 移動速度とアニメーション速度が一致していないと、足の滑り(ムーンウォークのような気持ち悪さ)が発生するので、
	// 再生速度の調整を行う。
	// 物理的に1m歩く間にアニメーションデータ内で何m歩くかを調整するための係数を adjustment_ratio として渡す
	// @param mov_vec 移動しようとする方向のベクトル(長さも含む)
	// @param anim Animatorコンポーネントへのポインタ
	// @param cur_velocity 現在の移動速度ベクトル
	// @param adjustment_ratio アニメーションデータ内の移動速度
	//--------------------------------------------------------------------------------------------------------------
	void EnemyRVOSystem::AdjustAnimationSpeedByMovementSpeed(Vector3 mov_vec, Animator* anim, const Vector3& cur_velocity, float adjustment_ratio)
	{
		if (!anim) return;
		Vector3 velocity = cur_velocity;

		velocity.y = 0; // Y軸の速度を無視する
		float mov_length_sqr = mov_vec.magnitudeSquared();
		float velocity_length_sqr = velocity.magnitudeSquared();

		//ゼロ割り防止
		if (mov_length_sqr <= 1e-6f * 1e-6f || velocity_length_sqr <= 1e-6f * 1e-6f) {
			anim->anim_speed = 0.01f; // 移動しない場合はアニメーション速度を最小値にする
			return;
		}

		//動こうとしている方向に対し、実際どれくらい進めているのかを計算する
		//正射影比 kを計算
		float m_dot_v = max(0.0f, mov_vec.dot(velocity));
		float k = m_dot_v / mov_length_sqr;

		//実速度の大きさによって再生速度を調整するパラメータを計算
		//lengthを使わない軽量版
		//Quake3で有名な高速逆平方根を使って、速度の大きさの近似値を計算する
		float inv_sqr_length = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(velocity_length_sqr)));
		//当たり前だが、x * 1/√x = x/√x = √x なので、速度の大きさの近似値を計算できる 
		float length_approx = velocity_length_sqr * inv_sqr_length; // length = sqrt(length^2) の近似値


		float speed_ratio = k * adjustment_ratio * length_approx;

		//アニメーション速度を調整する
		anim->anim_speed = max(0.0f, speed_ratio);
	}
}