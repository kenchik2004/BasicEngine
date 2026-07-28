#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{

	class Enemy;
	class TeamMemberEnemyBrain;

	//! @brief プレイヤーを取り囲む状態を管理するステートクラス
	//! @brief 敵メンバーがプレイヤーを取り囲む行動を行う際の状態を表す
	class EnemyTeamSiegeState :
		public IState
	{
	public:
		//! @brief コンストラクタ
		//! @param owner_enemy_ 所有者となるEnemyオブジェクトへのポインタ
		EnemyTeamSiegeState(Enemy* owner_enemy_);

		// 親クラスの純粋仮想クラスのオーバーライド
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		void DebugDraw() override;

	private:
		Enemy* owner_enemy;							// 所有者となるEnemyオブジェクトへのポインタ
		GameObject* player = nullptr;						// プレイヤーへのポインタ
		TeamMemberEnemyBrain* brain = nullptr;		// 所有者のAI脳へのポインタ
		bool team_released = false;					// チームが解散されたかどうかのフラグ

		RigidBody* rb = nullptr;						// 所有者のRigidBodyへのポインタ
		Animator* animator = nullptr;					// 所有者のAnimatorへのポインタ
		float siege_timer = 0.0f;						// チーム攻撃状態のタイマー

		static constexpr float SIEGE_DURATION = 10.0f; // チーム攻撃状態の持続時間
		static constexpr float SIEGE_RADIUS = 10.0f; // プレイヤーを取り囲む位置の半径
		static constexpr float SIEGE_COMPLETE_DISTANCE = 3.0f; // プレイヤーを取り囲む位置に到達したとみなす距離の閾値
		static constexpr float SIEGE_SPEED = 20.0f; // プレイヤーを取り囲む位置に向かう速度
		static constexpr float ROTATION_SPEED = 10.0f; //　プレイヤーを取り囲む位置に向かう際の回転速度

		static constexpr float SIEGE_RECALCULATION_DISTANCE = 10.0f; // 取り囲み中にプレイヤーが移動した場合に、取り囲み位置を再計算する距離の閾値

		Vector3  siege_position;						// 取り囲み位置の計算結果を保持する変数
		float distance_to_siege_position_sqr = 0.0f;			// 取り囲み位置までの距離を保持する変数の二乗値
		Vector3   last_player_position;					// 再計算前のプレイヤーの位置を保持する変数


		//! @brief プレイヤーを取り囲む位置を計算する関数
		//! @return 計算された取り囲み位置を示すVector3
		Vector3 CalculateSiegePosition();
		void CalculatePlayerAvoid(Vector3& move_dir, const Vector3& player_pos,const Vector3& owner_pos);

	};
}

