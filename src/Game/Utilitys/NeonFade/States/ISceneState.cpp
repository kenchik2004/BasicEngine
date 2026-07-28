// ISceneState.cpp
//---------------------------------------------------------------------------
//! @file   ISceneState.cpp
//! @brief  ISceneStateの実装。シーン状態の基底インタフェース処理を定義する
//---------------------------------------------------------------------------
#include "ISceneState.h"

namespace NeonFade {

	/// @brief コンストラクタ
	/// @param owner_scene_ 所有者となるシーンのポインタ
	ISceneState::ISceneState(Scene* owner_scene_)
	{
		// 所有者となるシーンをメンバ変数に保持する
		owner_scene = owner_scene_;
		// ステート名を空文字列で初期化する
		name = "";
	}

	/// @brief 登録された遷移要求を評価し次に遷移すべきステート名を返す
	/// @return 遷移先のステート名 遷移要求がない場合は空文字列を返す
	std::string ISceneState::GetChangeRequest()
	{
		// 登録されている全ての遷移要求を優先度順に確認する
		for (auto& request : change_requests) {
			// 優先度順にチェック
			//  関数が設定されていて、かつ関数がtrueを返したら状態遷移要求とみなす
			if (request->function && request->function()) {
				// 条件を満たした最初の遷移先ステート名を返す
				return request->state_name;
			}
		}
		// どの条件も満たさない場合は空文字列を返す
		return "";
	}

	/// @brief ステートの遷移要求とその条件を登録する
	/// @param state_name 遷移先のステート名
	/// @param func 遷移条件を判定する関数オブジェクト
	/// @param priority 遷移の優先度(数値が小さいほど優先される)
	void ISceneState::RegisterChangeRequest(const std::string& state_name, std::function<bool()>& func, u8 priority)
	{
		// 新しい遷移要求オブジェクトを生成する
		auto request = make_safe_unique<FunctionRequest>();
		// 関数オブジェクトをムーブして割り当てる
		request->function = std::move(func);
		// 優先度を設定する
		request->priority = priority;
		// 遷移先のステート名を設定する
		request->state_name = state_name;
		// 優先度順に挿入
		// 既存の要求リストの中から自身より優先度が低い(数値が大きい)位置を検索する
		auto insert_pos = std::find_if(change_requests.begin(), change_requests.end(),
			[priority](const SafeUniquePtr<FunctionRequest>& req) {
				return req->priority < priority;
			});
		// 見つけた位置に新しい遷移要求を挿入する
		change_requests.insert(insert_pos, std::move(request));

	}

}