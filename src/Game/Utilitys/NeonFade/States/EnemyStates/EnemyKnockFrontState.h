#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyKnockFrontState :
		public IState
	{
	public:
		//! @brief コンストラクタ
		EnemyKnockFrontState(Enemy* owner_enemy_);

		// IStateの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;

		//! @brief この状態から遷移可能な状態を制限するための関数
		bool CanTransitTo(const std::string& state_name) override;


		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
	private:

		//! @brief ノックバック中にコライダーの向きを変えるための関数
		void RotateCollider();

		Enemy* owner_enemy = nullptr;						//<! オーナーのEnemyオブジェクトへのポインタ
		Animator* animator = nullptr;						//<! オーナーのAnimatorコンポーネントへのポインタ
		RigidBody* rb = nullptr;							//<! オーナーのRigidBodyコンポーネントへのポインタ
		CapsuleCollider* col = nullptr;						//<! オーナーのColliderコンポーネントへのポインタ
		static constexpr float KNOCK_FRONT_DURATION = 3.0f;	//<! ノックバック状態の継続時間（秒）
		float elapsed_time = 0.0f;							//<! ステート開始後の経過時間を追跡するための変数

		static constexpr float HIT_STOP_DURATION = 0.2f;	//<! ヒットストップの継続時間（秒）
		static constexpr float HIT_STOP_START = 0.1f;		//<! ヒットストップを開始するタイミング（秒）
		bool hit_stop_started = false;						//<! ヒットストップが開始されたかどうかを追跡するフラグ
		bool hit_stop_ended = false;						//<! ヒットストップが終了したかどうかを追跡するフラグ
		Vector3 knock_front_velocity = { 0,0,0 };			//<! ヒットストップ直前の速度を保存するための変数


		static constexpr float KNOCK_BACK_DURATION = 2.0f;
		static constexpr float CHAIN_KNOCK_BACK_START_TIME = 0.5f; // チェインノック(連鎖して当たった他の敵も吹き飛ばす)判定の開始時間
		static constexpr float CHAIN_KNOCK_BACK_DURATION = 0.5f; // チェインノック(連鎖して当たった他の敵も吹き飛ばす)判定の開時間

		ColliderWP chain_knock_collider = nullptr; // チェインノックバックの判定用コライダー
		static constexpr float CHAIN_KNOCK_BACK_FORCE = 20.0f; // チェインノックバックの力
		static constexpr float CHAIN_KNOCK_BACK_UP_FORCE = 10.0f; // チェインノックバックの上方向の力
		static constexpr u32 CHAIN_KNOCK_BACK_DAMAGE = 5; // チェインノックバックのダメージ


	};
}

