#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
namespace NeonFade
{
	//! @brief 敵がカバーしている状態を管理するクラス
	//! 敵が仲間をカバーしている状態を管理するクラス。
	//! カバー中は、敵は仲間とプレイヤーの間に位置し、プレイヤーからの攻撃を引き受ける役割を果たす。

	class Enemy;
	class EnemyCoverState :
		public IState
	{
	public:
		EnemyCoverState(Enemy* owner_enemy_);
		// IStateの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;

		//! @brief この状態から遷移可能な状態を制限するための関数
		bool CanTransitTo(const std::string& state_name) override;
	private:
		static constexpr float COVER_DURATION = 2.0f;				//<! カバー状態の持続時間（秒）
		static constexpr float ROTATION_SPEED = 1.0f;					//<! カバー中の回転の補間速度
		static constexpr float COHESION_WEIGHT = 0.5f;					//<! カバー中の仲間との凝集力の重み
		static constexpr float COVER_CONTINUE_ANGLE_THRESHOLD = 40.0f;	//<! カバーを続けるための角度の閾値（度）
		static constexpr float TARGET_DISTANCE_THRESHOLD = 50.0f;				//<! カバー対象が離脱完了したとみなす距離の閾値


		float elapsed_time = 0.0f;							//<! ステート開始後の経過時間を追跡するための変数

		Enemy* owner_enemy = nullptr;						//<! オーナーのEnemyオブジェクトへのポインタ
		Enemy* covering_enemy = nullptr;						//<! カバー中のEnemyオブジェクトへのポインタ
		Animator* animator = nullptr;						//<! オーナーのAnimatorコンポーネントへのポインタ
		RigidBody* rb = nullptr;							//<! オーナーのRigidBodyコンポーネントへのポインタ

	};
}

