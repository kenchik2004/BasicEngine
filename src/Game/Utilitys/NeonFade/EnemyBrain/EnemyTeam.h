//---------------------------------------------------------------------------
//! @file   EnemyTeam.h
//! @brief  敵チームの管理・連携処理クラスの定義
//---------------------------------------------------------------------------
#pragma once
#include "Game/Utilitys/NeonFade/EnemyBrain/TeamMemberEnemyBrain.h"
#include "Game/Utilitys/NeonFade/EnemyBrain/LeaderEnemyBrain.h"
namespace NeonFade {

	//! @brief 敵チームの管理・連携処理用クラス
	//! @brief 敵チームのリーダーやメンバー同士が互いに参照し合う為のブラックボード的な役割を持つクラス
	class EnemyTeam
	{
	private:
		LeaderEnemyBrain* leader_brain;				//<! チームのリーダーの思考クラスへのポインタ
		std::vector<TeamMemberEnemyBrain*> members;	//<! チームメンバーの思考クラスへのポインタのvector
	public:

		//! @brief デストラクタ
		virtual ~EnemyTeam();

		//! @brief チームのメンバー数を取得する
		//! @return チームのメンバー数
		u64 GetMemberNum() const {
			return members.size();
		}

		//! @brief チームのリーダーを取得する
		//! @return チームのリーダーの思考クラスへのポインタ
		LeaderEnemyBrain* GetLeader() const {
			return leader_brain;
		}

		//! @brief チームのメンバーを取得する
		//! @return チームのメンバーの思考クラスへのポインタのvector参照
		const std::vector<TeamMemberEnemyBrain*>& GetMembers() const {
			return members;
		}

		//! @brief チームにメンバーを追加する
		//! @brief メンバーの思考クラスへのポインタを引数に取り、チームに追加する
		//! @param member 追加するメンバーの思考クラスへのポインタ
		void AddMember(TeamMemberEnemyBrain* member);

		//! @brief チームのリーダーを設定する
		//! @brief リーダーの思考クラスへのポインタを引数に取り、チームのリーダーを設定する
		//! @param leader 設定するリーダーの思考クラスへのポインタ
		void SetLeader(LeaderEnemyBrain* leader);

		//! @brief チームからメンバーを削除する
		//! @brief メンバーの思考クラスへのポインタを引数に取り、チームから削除する
		//! @param member 削除するメンバーの思考クラスへのポインタ
		void SubMember(TeamMemberEnemyBrain* member);

		//! @brief チームを解散する
		//! @brief チームのリーダーとメンバーの参照をクリアし、チームを解散する
		void ClearTeam();
	};
}

