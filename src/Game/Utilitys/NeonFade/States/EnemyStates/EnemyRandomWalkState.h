#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyRandomWalkState :
		public IState
	{
	public:
		EnemyRandomWalkState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		bool CanTransitTo(const std::string& state_name) override;
		void DebugDraw() override;

	private:
		static constexpr float ROTATION_SPEED = 0.01f; // 回転の補間速度
		static constexpr float RANDOM_WALK_RADIUS = 5.0f; // ランダムな目的地を生成する範囲の半径
		static constexpr float DEST_THRESHOLD = 5.0f;  // 目的地に近いとみなす距離

		Vector3 next_destination;						//<! 次の目的地
		static constexpr float WALK_SPEED = 7.5f;						//<! 散歩の速度

		static constexpr float MAX_WALK_TIME = 10.0f;						//<! 散歩の最大時間
		float elapsed_time = 0.0f;							//<! 散歩開始からの経過時間

		Enemy* owner_enemy = nullptr;						//<! オーナーのEnemyオブジェクトへのポインタ
		Animator* animator = nullptr;						//<! オーナーのAnimatorコンポーネントへのポインタ
		RigidBody* rb = nullptr;							//<! オーナーのRigidBodyコンポーネントへのポインタ


		//! @brief 基礎の移動方向を計算する関数
		void CalculateBaseMovement(Vector3& out_mov_dir);
		//! @brief 近くの敵に引き寄せられる方向を計算する関数
		void CalculateCohesion(Vector3& out_mov_dir);
		//! @brief 最終的な移動方向を正規化し、速度と回転を適用する関数
		void ApplyMovementAndRotation(Vector3& mov_dir);


	};
}
