#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{

	class Enemy;

	// @brief 敵のドロップキック攻撃のエントリーステート
	// @brief このステートは、敵がプレイヤーに向かってドロップキック攻撃を開始する前の準備段階を表す
	// @brief プレイヤーに向かってダッシュする動作を行い、攻撃の準備が整ったら次のステートに遷移する
	class EnemyDropKickAttackEntryState :
		public IState
	{
	public:
		// @brief コンストラクタ
		// @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyDropKickAttackEntryState(Enemy* owner_enemy_);

		// @brief IStateの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;

		// @brief この状態から遷移可能な状態を制限するための関数
		bool CanTransitTo(const std::string& state_name) override;
	private:
		Enemy* owner_enemy = nullptr;	//<! 所有者となるEnemyオブジェクトへのポインタ
		Animator* animator = nullptr;	//<! アニメーターへのポインタ
		RigidBody* rb = nullptr;		//<! 物理コンポーネントへのポインタ
		Vector3 to_target = { 0,0,0 };	//<! プレイヤーへの方向ベクトル

		float attack_timer = 0.0f;		//<! 攻撃状態の経過時間

		static constexpr float RUN_SPEED = 20.0f; // 攻撃状態の移動速度(m/s)
		static constexpr float ROTATION_SPEED = 20.0f; // 攻撃状態の回転速度

		static constexpr float ENTRY_DURATION = 7.0f; // 攻撃状態の持続時間（秒）
		static constexpr float ATTACK_TRANSIT_RANGE = 20.0f; // 攻撃を開始できるプレイヤーとの距離、射程範囲(m)

	};


	// @brief 敵のドロップキック攻撃のメインステート
	// @brief このステートは、敵がプレイヤーに向かってドロップキック攻撃を行う状態を表す
	// @brief ジャンプしながらドロップキックを行い、攻撃が終了したら次のステートに遷移する
	class EnemyDropKickAttackMainState :
		public IState
	{
	public:
		// @brief コンストラクタ
		// @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyDropKickAttackMainState(Enemy* owner_enemy_);

		// @brief IStateの純粋仮想関数のオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;

		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;

		// @brief この状態から遷移可能な状態を制限するための関数
		bool CanTransitTo(const std::string& state_name) override;
	private:
		Enemy* owner_enemy = nullptr;	//<! 所有者となるEnemyオブジェクトへのポインタ
		Animator* animator = nullptr;	//<! アニメーションコンポーネントへのポインタ
		RigidBody* rb = nullptr;		//<! 物理コンポーネントへのポインタ
		CapsuleCollider* col = nullptr;	//<! コライダーコンポーネントへのポインタ
		SphereColliderWP attack_col;	//<! 攻撃判定用のコライダーへの弱参照

		float attack_timer = 0.0f;
		static constexpr float ATTACK_DURATION = 3.0f; //<! 攻撃状態の持続時間（秒）

		static constexpr float ATTACK_SPEED = 30.0f; //<! 攻撃中の移動速度(m/s)
		static constexpr float ATTACK_SPEED_Y = 20.0f; //<! 攻撃中のジャンプ速度(m/s)


		float initial_collision_height = 0.0f; //<! 元のコリジョンの高さ
		static constexpr float EDITED_COLLISION_HEIGHT = 3.25f; //<! 攻撃中に適用されるコリジョンの高さ
		static constexpr float COLLISION_EDIT_START_TIME = 0.95f; //<! コリジョンの高さ変更を開始する時間（秒）
		static constexpr float COLLISION_EDIT_END_TIME = 1.15f; //<! コリジョンの高さ変更を終了する時間（秒）
		static constexpr float COLLISION_RESTORE_START_TIME = 1.95f; //<! コリジョンの高さを元に戻し始める時間(秒)
		static constexpr float COLLISION_RESTORE_END_TIME = 2.15f; //<! コリジョンの高さを元に戻し終わる時間(秒)

		static constexpr float INVINCIBILITY_START_TIME = 0.2f; //<! 無敵状態を開始する時間（秒）
		static constexpr float INVINCIBILITY_END_TIME = 0.7f; //<! 無敵状態を終了する時間（秒）

		// @brief コリジョンの高さを変更する関数
		void ChangeCollisionHeight();

		// @brief コリジョンの高さを補間して変更する関数
		// @param start_time 変更を開始する時間
		// @param end_time 変更を終了する時間
		// @param current_time 現在の時間
		// @param initial_height 初期のコリジョンの高さ
		// @param target_height 目標のコリジョンの高さ
		void EditCollisionHeight(const float start_time, const float end_time, const float current_time, const float initial_height, const float target_height);

	};
}
