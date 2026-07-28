// IStateMachine.h
#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h" 
class GameObject;

namespace NeonFade
{
	/// @brief 各種ゲームオブジェクトの状態遷移を管理する基底クラス
	class IStateMachine
	{
	public:
		/// @brief デストラクタ
		virtual ~IStateMachine() = default;
		/// @brief コンストラクタ
		/// @param owner_ 所有者となるゲームオブジェクトのポインタ
		IStateMachine(GameObject* owner_);


		/// @brief 次のステートへ状態を切り替える
		/// @param next 遷移先となるステートの名前
		virtual void ChangeState(std::string_view next);
		/// @brief 現在のステート名を取得する
		/// @return 現在のステート名を示す文字列
		const std::string GetCurrentStateName() const { return current_state ? current_state->GetName() : "none"; }
		/// @brief 現在実行中のステートを取得する
		/// @return 現在のステートへのポインタ
		IState* GetCurrentState() const { return current_state; }
		/// @brief 指定した名前のステートを取得する
		/// @param name 取得したいステートの名前
		/// @return 指定したステートへのポインタ
		IState* GetState(std::string_view name) const;
		/// @brief 毎フレームの更新処理を行う
		/// @param dt 経過時間
		virtual void Update(float dt);
		/// @brief 新しいステートを登録する
		/// @param name 登録するステートの名前
		/// @param state 登録するステートのスマートポインタ
		void AddState(std::string_view name, SafeUniquePtr<IState> state);

		/// @brief 登録されたステートを削除する
		/// @param name 削除するステートの名前
		void RemoveState(std::string_view name);
		/// @brief 衝突開始イベントを処理する
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnCollisionEnter(const HitInfo& hit_info);
		/// @brief 衝突継続イベントを処理する
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnCollisionStay(const HitInfo& hit_info);
		/// @brief 衝突終了イベントを処理する
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnCollisionExit(const HitInfo& hit_info);
		/// @brief トリガー領域への侵入イベントを処理する
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnTriggerEnter(const HitInfo& hit_info);
		/// @brief トリガー領域での継続イベントを処理する
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnTriggerStay(const HitInfo& hit_info);
		/// @brief トリガー領域からの退出イベントを処理する
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnTriggerExit(const HitInfo& hit_info);
		/// @brief デバッグ用の描画処理を行う
		virtual void DebugDraw();
	protected:
		IState* current_state = nullptr; /// @brief 現在実行中のステートへのポインタ
		GameObject* owner = nullptr; /// @brief 所有者となるゲームオブジェクトへのポインタ
		std::unordered_map<std::string, SafeUniquePtr<IState>> states; /// @brief 登録されたステートを管理する連想配列
	};
}