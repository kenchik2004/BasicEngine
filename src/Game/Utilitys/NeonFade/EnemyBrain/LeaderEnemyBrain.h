//---------------------------------------------------------------------------
//! @file   LeaderEnemyBrain.h
//! @brief  リーダー敵AI思考クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/AbstractEnemyBrain.h"

namespace NeonFade {

	// 各種前方宣言
	class Enemy;
	class EnemyStateMachine;
	class TeamMemberEnemyBrain;
	class EnemyTeam;

	//! @brief リーダー敵の思考を管理するクラス
	//! @brief リーダー敵は、チームのメンバーに指示を出すことができる特殊な敵である
	class LeaderEnemyBrain :
		public AbstractEnemyBrain
	{
	public:

		//! @brief リーダー敵AIのコンストラクタ
		//! @param state_machine_ 状態遷移を管理するステートマシン
		//! @param player_ プレイヤーの情報を持つ弱参照ポインタ
		//! @param team 所属するチームのポインタ
		LeaderEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_, EnemyTeam* team);

		//! @brief デストラクタ
		~LeaderEnemyBrain();

		//! @brief 思考処理を実行する
		std::string Think() override;

		//! @brief 死亡時の処理
		//! @brief 死亡時には追加の処理が発生するので、親クラスのDie関数をオーバーライドする
		void Die() override;

		//! @brief デバッグ描画
		void DebugDraw() override {}

		//! @brief チームへの参照を削除する
		//! @brief チームへの参照を削除するだけで、チーム側の情報は変更されないので注意すること
		void ResetTeamRef();

		//! @brief 所属するチーム参照を設定する
		//! @brief リーダーが持っている参照を削除するだけで、チーム側の情報は変更されないので注意すること
		//! @param team 設定するチームのポインタ
		void SetTeamRef(EnemyTeam* team);

		//! @brief 所属するチームを取得する
		//! @return 所属するチームのポインタ
		const EnemyTeam* GetTeam() const { return my_team; }

		static constexpr u32 TEAM_RELEASE_MEMBER_NUM = 3;	//<! チームを解散する際に、メンバーがこの人数以下であれば解散する

	private:
		EnemyTeam* my_team = nullptr;		//<! 所属するチームのポインタ
		static constexpr u32 MAX_HP = 30;	//<! リーダー敵の最大HP

		float instruct_cooldown_timer = 0.0f;	//<! 指示を出すクールダウンタイマー
		static constexpr float INSTRUCT_COOLDOWN = 10.0f;	//<! 指示を出せるクールダウン時間

		//! @brief チームを解散するか新しいリーダーを立てる
		void ReleaseTeamOrSelectNewLeader();

	};
}

