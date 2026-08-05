// IStateMachine.cpp
#include "IStateMachine.h"
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	/// @brief コンストラクタ
	/// @param owner_ 所有者となるゲームオブジェクトのポインタ
	IStateMachine::IStateMachine(GameObject* owner_)
	{
		// 所有者となるゲームオブジェクトをメンバ変数に保持する
		owner = owner_;
	}
	/// @brief 次のステートへ状態を切り替える
	/// @param next 遷移先となるステートの名前
	void IStateMachine::ChangeState(std::string_view next)
	{
		// 指定された名前のステートを連想配列から検索する
		auto it = states.find(next.data());
		// ステートが見つかった場合の処理を実行する
		if (it != states.end())
		{
			auto cur_state = current_state;
			current_state = nullptr;
			// 現在のステートが存在する場合は終了処理を呼び出す
			if (cur_state)
			{
				cur_state->OnExit(this);
			}
			// 新しいステートを現在のステートとして設定する
			current_state = it->second.get();
			// 新しいステートの開始処理を呼び出す
			if (current_state)
			{
				current_state->OnEnter(this);
			}
		}
	}

	/// @brief 指定した名前のステートを取得する
	/// @param name 取得したいステートの名前
	/// @return 指定したステートへのポインタ 見つからない場合はnullptrを返す
	IState* IStateMachine::GetState(std::string_view name) const
	{
		// 指定された名前のステートを連想配列から検索する
		auto it = states.find(name.data());
		// ステートが見つかった場合はそのポインタを返す
		if (it != states.end())
		{
			return it->second.get();
		}
		// 見つからなかった場合はnullptrを返す
		return nullptr;
	}

	/// @brief 毎フレームの更新処理を行う
	/// @param dt 経過時間
	void IStateMachine::Update(float dt)
	{
		// 現在のステートが有効な場合のみ更新処理を行う
		if (current_state)
		{
			// 現在のステートの更新処理を呼び出す
			current_state->Update(this, dt);
			// 現在のステートから次のステートへの遷移要求を取得する
			auto&& request_state = current_state->GetChangeRequest();
			// 遷移要求が存在する場合はステートの切り替えを行う
			if (!request_state.empty())
			{
				ChangeState(request_state);
			}
		}
	}

	/// @brief 新しいステートを登録する
	/// @param name 登録するステートの名前
	/// @param state 登録するステートのスマートポインタ
	void IStateMachine::AddState(std::string_view name, SafeUniquePtr<IState> state) {
		// ステートが有効であれば名前を設定する
		if (state)
			state->SetName(name);
		// 連想配列にステートを登録する
		// もし同じ名前のステートが既に存在する場合は登録しない
		if (states.find(name.data()) != states.end())
		{
			return;
		}

		states[name.data()] = std::move(state);
	}

	/// @brief 登録されたステートを削除する
	/// @param name 削除するステートの名前
	void IStateMachine::RemoveState(std::string_view name) {
		// 指定された名前のステートを連想配列から削除する
		states.erase(name.data());
	}
	/// @brief デバッグ用の描画処理を行う
	void IStateMachine::DebugDraw()
	{
		// 現在のステートが有効な場合のみデバッグ情報を描画する
		if (current_state)
		{
			// 現在のオブジェクト名とステート名を画面に出力する
			printfDx("%s Current State: %s\n", owner->name.c_str(), current_state->GetName().c_str());
			// 現在のステート自身のデバッグ描画処理を呼び出す
			current_state->DebugDraw();
		}
	}

	/// @brief 衝突開始イベントを処理する
	/// @param hit_info 衝突に関する詳細情報
	void IStateMachine::OnCollisionEnter(const HitInfo& hit_info)
	{
		// 現在のステートが無効であれば処理を中断する
		if (!current_state) return;
		// 現在のステートに衝突開始イベントを伝達する
		current_state->OnCollisionEnter(this, hit_info);
	}
	/// @brief 衝突継続イベントを処理する
	/// @param hit_info 衝突に関する詳細情報
	void IStateMachine::OnCollisionStay(const HitInfo& hit_info)
	{
		// 現在のステートが無効であれば処理を中断する
		if (!current_state) return;
		// 現在のステートに衝突継続イベントを伝達する
		current_state->OnCollisionStay(this, hit_info);
	}
	/// @brief 衝突終了イベントを処理する
	/// @param hit_info 衝突に関する詳細情報
	void IStateMachine::OnCollisionExit(const HitInfo& hit_info)
	{
		// 現在のステートが無効であれば処理を中断する
		if (!current_state) return;
		// 現在のステートに衝突終了イベントを伝達する
		current_state->OnCollisionExit(this, hit_info);
	}
	/// @brief トリガー領域への侵入イベントを処理する
	/// @param hit_info 衝突に関する詳細情報
	void IStateMachine::OnTriggerEnter(const HitInfo& hit_info)
	{
		// 現在のステートが無効であれば処理を中断する
		if (!current_state) return;
		// 現在のステートにトリガー侵入イベントを伝達する
		current_state->OnTriggerEnter(this, hit_info);
	}
	/// @brief トリガー領域での継続イベントを処理する
	/// @param hit_info 衝突に関する詳細情報
	void IStateMachine::OnTriggerStay(const HitInfo& hit_info)
	{
		// 現在のステートが無効であれば処理を中断する
		if (!current_state) return;
		// 現在のステートにトリガー継続イベントを伝達する
		current_state->OnTriggerStay(this, hit_info);
	}
	/// @brief トリガー領域からの退出イベントを処理する
	/// @param hit_info 衝突に関する詳細情報
	void IStateMachine::OnTriggerExit(const HitInfo& hit_info)
	{
		// 現在のステートが無効であれば処理を中断する
		if (!current_state) return;
		// 現在のステートにトリガー退出イベントを伝達する
		current_state->OnTriggerExit(this, hit_info);
	}
}