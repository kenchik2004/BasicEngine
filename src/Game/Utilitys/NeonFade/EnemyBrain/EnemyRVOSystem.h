#pragma once
#include "Game/Objects/NeonFade/Enemy.h"
namespace NeonFade {

	//! @brief 敵のRVO（Reciprocal Velocity Obstacles）システムを管理するクラス
	//! @note RVOは、複数の敵が互いに衝突しないように移動するためのアルゴリズムであり、近くの敵との相互作用を考慮して移動方向を調整する
	//! @
	class EnemyRVOSystem
	{
	public:


		//! @brief 近くの敵に引き寄せられる方向を計算する関数
		//! @param out_mov_dir 計算結果の移動方向を格納するVector3参照
		//! @param transform 自身のTransformポインタ
		//! @param owner_enemy 自身のEnemyポインタ
		static void CalculateCohesion(Vector3& out_mov_dir, const Transform* transform, const Enemy* owner_enemy, float cohesion_weight = 1.0f);


		static void SmoothMovement(Vector3& mov_dir, const Vector3& forward, float smoothing_factor, float dt);
		//! @brief 最終的な移動方向を正規化し、速度を適用する関数
		//! @param mov_dir 計算された移動方向を格納するVector3参照
		//! @param rb 自身のRigidBodyポインタ
		//! @param movement_factor 移動速度の係数
		static void ApplyMovement(Vector3& mov_dir, RigidBody* rb, const float movement_factor);


		//! @brief 最終的な移動方向を正規化し、回転を適用する関数
		//! @param mov_dir 計算された移動方向を格納するVector3参照
		//! @param transform 自身のTransformポインタ
		//! @param rotation_factor 回転速度の係数
		static void ApplyRotation(Vector3& mov_dir, Transform* transform, const float rotation_factor);

		//! @brief 最終的な移動方向を正規化し、速度と回転を適用する関数
		//! @brief この関数は、ApplyMovementとApplyRotationを同時に呼び出すよりも最適化されているため、移動と回転の両方を同時に適用する場合に使用することが推奨される
		//! @param mov_dir 計算された移動方向を格納するVector3参照
		//! @param transform 自身のTransformポインタ
		//! @param rb 自身のRigidBodyポインタ
		//! @param rotation_factor 回転速度の係数
		//! @param movement_factor 移動速度の係数
		static void ApplyMovementAndRotation(Vector3& mov_dir, Transform* transform, RigidBody* rb, const float rotation_factor, const float movement_factor);

		//--------------------------------------------------------------------------------------------------------------
		// @brief アニメーション速度を移動速度に応じて調整する関数
		// 移動速度とアニメーション速度が一致していないと、足の滑り(ムーンウォークのような気持ち悪さ)が発生するので、
		// 再生速度の調整を行う。
		// 物理的に1m歩く間にアニメーションデータ内で何m歩くかを調整するための係数を adjustment_ratio として渡す
		// @param mov_vec 移動しようとする方向のベクトル(長さも含む)
		// @param anim Animatorコンポーネントへのポインタ
		// @param cur_velocity 現在の移動速度ベクトル
		// @param adjustment_ratio アニメーション速度の調整係数
		//--------------------------------------------------------------------------------------------------------------
		static void AdjustAnimationSpeedByMovementSpeed(Vector3 mov_vec, Animator* anim, const Vector3& cur_velocity, float adjustment_ratio);
	};


}
