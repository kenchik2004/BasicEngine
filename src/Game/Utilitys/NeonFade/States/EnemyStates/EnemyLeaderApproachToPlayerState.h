#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{

	class Enemy;
	//! @brief プレイヤーに近づく状態を管理するステートクラス
	//! @brief 敵リーダーが指示を出す前の中間状態として使用する
	class EnemyLeaderApproachToPlayerState :
		public IState
	{
	public:

		//! @brief コンストラクタ
		//! @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyLeaderApproachToPlayerState(Enemy* owner_enemy_);

		// 親クラスの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;

		//! @brief 状態遷移の可否を判定する関数
		bool CanTransitTo(const std::string& state_name) override;

	private:
		Enemy* owner_enemy; //!< 所有者となるEnemyオブジェクトへのポインタ
		RigidBody* rb; //!< 所有者のRigidBodyへのポインタ
		Animator* animator; //!< 所有者のAnimatorへのポインタ
		Transform* player_trns; //!< プレイヤーのTransformへのポインタ

		Vector3 goal_offset = { 0,0,0 }; //!< プレイヤーに近づく際の目標位置のオフセット

		static constexpr float APPROACH_SPEED = 20.0f; //!< プレイヤーに近づく際の移動速度
		static constexpr float ROTATION_SPEED = 10.0f; //!< プレイヤーに近づく際の回転速度
		static constexpr float PLAYER_LOST_DISTANCE_THRESHOLD = 70.0f; //!< プレイヤーを見失う距離の閾値
		static constexpr float PLAYER_APPROACH_DISTANCE_THRESHOLD = 5.0f; //!< 近づいたとみなす距離の閾値
		static constexpr float RANDOM_OFFSET_LENGTH = 40.0f; //!< プレイヤーに近づく際の目標位置のオフセットベクトルの長さ

		float player_lost_timer = 0.0f; //!< プレイヤーを見失った時間の計測用タイマー
		static constexpr float PLAYER_LOST_TIME_THRESHOLD = 3.0f; //!< プレイヤーを見失ったとみなす時間の閾値

		static constexpr float ANIMATION_ADJUSTMENT_RATIO = 0.05f; //!< アニメーション速度の調整係数

		float distance_to_player_sqr = 0.0f; //!< プレイヤーまでの距離の二乗値

	};
}
