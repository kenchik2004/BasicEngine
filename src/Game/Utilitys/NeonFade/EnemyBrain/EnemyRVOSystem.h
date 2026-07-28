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
		static void CalculateCohesion(Vector3& out_mov_dir, const Transform* transform, const Enemy* owner_enemy);

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

	};

}
