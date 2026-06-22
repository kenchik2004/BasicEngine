#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade {
	//! @brief 敵がカバーしに近づく状態を管理するクラス
	//! @details この状態では、敵は見方をカバーするために近づき
	//! プレイヤーからの攻撃を避けるための行動を取る
	//! カバー位置に到達すると、カバー状態に遷移する

	class Enemy;
	class EnemyCoverApproachState :
		public IState
	{
	public:
		EnemyCoverApproachState(Enemy* owner_enemy_);
		// IStateの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		void DebugDraw() override;

		//! @brief この状態から遷移可能な状態を制限するための関数
		bool CanTransitTo(const std::string& state_name) override;
		Enemy* GetCoverTarget() const { return cover_target; }	//<! カバーする対象のEnemyオブジェクトへのポインタを返す関数

	private:
		static constexpr float APPROACH_SPEED = 12.0f;	//<! カバーに近づく速度
		static constexpr float ROTATION_SPEED = 0.01f;	//<! カバーに近づく際の回転速度
		static constexpr float APPROACH_POSITION_RATIO = 0.6f;	//<! カバー位置に近づく際、プレイヤーと敵の距離の割合(0.0f~1.0f)
		static constexpr float APPROACH_DISTANCE_THRESHOLD = 5.0f;	//<! カバー位置に近づいたとみなす距離の閾値
		static constexpr float APPROACH_DESTINATION_RADIUS = 5.0f;	//<! カバー位置の周囲でランダムに目的地を設定する半径
		static constexpr float MAX_APPROACH_TIME = 10.0f;	//<! カバーに近づく最大時間

		Enemy* owner_enemy = nullptr;						//<! オーナーのEnemyオブジェクトへのポインタ
		Enemy* cover_target = nullptr;						//<! カバーする対象のEnemyオブジェクトへのポインタ
		RigidBody* rb = nullptr;							//<! オーナーのRigidBodyコンポーネントへのポインタ
		Animator* animator = nullptr;							//<! オーナーのAnimatorコンポーネントへのポインタ

		float elapsed_time = 0.0f;								//<! この状態に入ってからの経過時間
		Vector3 approach_destination;							//<! カバー位置に近づくための目的地

		//< @brief カバー位置に近づくための目的地を計算する関数
		void CalculateApproachDestination();	
		//< @brief カバー位置に近づくための移動方向を計算する関数
		void CalculateBaseMovement(Vector3& out_mov_dir);	
		//< @brief 近くの敵に引き寄せられる方向を計算する関数
		void CalculateCohesion(Vector3& out_mov_dir);	
		//< @brief 最終的な移動方向を正規化し、速度と回転を適用する関数
		void ApplyMovementAndRotation(Vector3& mov_dir);	
	};
}

