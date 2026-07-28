//---------------------------------------------------------------------------
//! @file   TeamMemberEnemyBrain.h
//! @brief  チームメンバー敵AI思考クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"


namespace NeonFade {

	//! @brief チームメンバー敵の思考を管理するクラス
	class EnemyStateMachine;
	class LeaderEnemyBrain;
	class EnemyTeam;

	//! @brief チームメンバー敵の思考を管理するクラス
	//! @brief チームメンバー敵は、リーダー敵の指示に従って行動する特殊な敵である
	class TeamMemberEnemyBrain :
		public AbstractEnemyBrain
	{
	public:

		//! @brief チームメンバー敵AIのコンストラクタ
		//! @param state_machine_ 状態遷移を管理するステートマシン
		//! @param player_ プレイヤーの情報を持つ弱参照ポインタ
		//! @param team 所属するチームのポインタ
		TeamMemberEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team);

		//! @brief デストラクタ
		~TeamMemberEnemyBrain();

		//! @brief 思考処理を実行する
		std::string Think() override;

		//! @brief デバッグ描画
		void DebugDraw() override;

		//! @brief 攻撃に向かうフラグを立てる(主にリーダーからの指示で呼ばれる)
		void GoToAttack() { go_to_attack = true; } // 攻撃に向かうフラグを立てる
		
		//! @brief リーダーになるフラグを立てる(主にリーダーからの指示で呼ばれる)
		void BecomeLeader() { become_leader = true; } // リーダーになるフラグを立てる

		//! @brief チームへの参照を削除する
		//! @brief チームへの参照を削除するだけで、チーム側の情報は変更されないので注意すること
		void ResetTeamRef();

		//! @brief 所属するチーム参照を設定する
		//! @brief メンバーが持っている参照を削除するだけで、チーム側の情報は変更されないので注意すること
		//! @param team 設定するチームのポインタ
		void SetTeamRef(EnemyTeam* team);

		//! @brief 所属するチームを取得する
		//! @return 所属するチームのポインタ
		EnemyTeam* GetTeam() const { return my_team; }

		//! @brief 死亡時の処理
		//! @brief 死亡時には追加の処理が発生するので、親クラスのDie関数をオーバーライドする
		void Die() override;


	private:
		EnemyStateMachine* machine = nullptr;
		EnemyTeam* my_team = nullptr;
		static constexpr u32 MAX_HP = 100;

		bool go_to_attack = false; // 攻撃に向かうかどうかのフラグ
		bool become_leader = false; // リーダーになるかどうかのフラグ

		//! @brief チームを解散するか新しいリーダーを立てる
		void ReleaseTeamOrSelectNewLeader();
	};
}

