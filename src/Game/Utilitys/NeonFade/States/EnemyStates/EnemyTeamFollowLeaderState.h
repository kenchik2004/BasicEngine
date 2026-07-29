#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{

	class Enemy;

	//! @brief チームのリーダーに付いていく状態を管理するステートクラスの基底クラス
	//! @brief リーダーからの指示が特にない場合に、待機状態の代わりとして使用する
	class EnemyTeamFollowLeaderState :
		public IState
	{
	public:
		//! @brief コンストラクタ
		//! @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyTeamFollowLeaderState(Enemy* owner_enemy_);

		// 親クラスの純粋仮想関数のオーバーライド
		void  Update(IStateMachine* machine, float dt) override;
		void  OnExit(IStateMachine* machine) override;
		//開始処理は派生クラスでオーバーライドする

	protected:
		float follow_speed = 0.0f; //!< リーダーに付いていく際の移動速度
		float rotation_speed = 0.0f; //!< リーダーに付いていく際の回転速度
		float distance_to_leader_sqr = 0.0f; //!< リーダーまでの距離の二乗値
		float animation_adjustment_ratio = 0.0f; //!< アニメーション速度の調整係数


		Enemy* owner_enemy; //!< 所有者となるEnemyオブジェクトへのポインタ
		RigidBody* rb; //!< 所有者のRigidBodyへのポインタ
		Animator* animator; //!< 所有者のAnimatorへのポインタ

	};

	class EnemyTeamFollowLeaderWalkState :
		public EnemyTeamFollowLeaderState
	{
	public:
		//! @brief コンストラクタ
		//! @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyTeamFollowLeaderWalkState(Enemy* owner_enemy_);

		// 開始処理のみをオーバーライドし、再生アニメーションを変更する
		void  OnEnter(IStateMachine* machine) override;

		//! @brief 状態遷移の可否を判定する関数
		//! @param state_name 遷移先のステート名
		bool CanTransitTo(const std::string& state_name) override;


		//! @brief デバッグ描画
		void DebugDraw() override;
	private:
		static constexpr float WALK_SPEED = 7.5f; //!< リーダーに付いていく際の移動速度
		static constexpr float ROTATION_SPEED = 10.0f; //!< リーダーに付いていく際の回転速度
		static constexpr float TO_RUN_DISTANCE_THRESHOLD = 30.0f; //!< リーダーに付いていく際の距離の閾値(この距離を超えると走る)
		static constexpr float FINISH_DISTANCE_THRESHOLD = 5.0f; //!< リーダーに付いていく際の距離の閾値の二乗値(この距離内ならリーダーに付いていくのを終了する)
		static constexpr float ANIMATION_ADJUSTMENT_RATIO = 0.09f; //!< アニメーション速度の調整係数

	};

	//! @brief チームのリーダーに付いていく状態を管理するステートクラス
	//! @brief リーダーからの指示が特にない場合に、待機状態の代わりとして使用する
	//!	@brief 通常のフォローステートではリーダーに付いていけない場合に移動速度を上げてリーダーに付いていく
	class EnemyTeamFollowLeaderRunState :
		public EnemyTeamFollowLeaderState
	{
	public:
		//! @brief コンストラクタ
		//! @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyTeamFollowLeaderRunState(Enemy* owner_enemy_);

		// 開始処理のみをオーバーライドし、再生アニメーションを変更する
		void  OnEnter(IStateMachine* machine) override;

		//! @brief 状態遷移の可否を判定する関数
		//! @param state_name 遷移先のステート名
		bool CanTransitTo(const std::string& state_name) override;


		//! @brief デバッグ描画
		void DebugDraw() override;
	private:
		static constexpr float FOLLOW_LEADER_RUN_SPEED = 20.0f; //!< リーダーに付いていく際の移動速度
		static constexpr float FOLLOW_LEADER_RUN_DISTANCE_THRESHOLD = 20.0f; //!< リーダーに付いていく際の距離の閾値
		static constexpr float FOLLOW_LEADER_RUN_ROTATION_SPEED = 15.0f; //!< リーダーに付いていく際の回転速度
		static constexpr float RUN_ANIMATION_ADJUSTMENT_RATIO = 0.05f; //!< アニメーション速度の調整係数

	};
}
