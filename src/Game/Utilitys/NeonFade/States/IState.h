// IState.h
//---------------------------------------------------------------------------
//! @file   IState.h
//! @brief  ステートの基底インタフェースの定義
//---------------------------------------------------------------------------
#pragma once

class GameObject;
namespace NeonFade
{
	class IStateMachine;
	/// @brief 汎用ステートマシン内で使用する各ステートの基底クラス
	class IState
	{
	public:
		/// @brief デストラクタ
		virtual ~IState() = default;
		/// @brief コンストラクタ
		/// @param owner_ 所有者となるゲームオブジェクトのポインタ
		IState(GameObject* owner_);
		/// @brief ステート開始時に呼ばれる処理
		/// @param machine 呼び出し元のステートマシン
		virtual void OnEnter(IStateMachine* machine) = 0;
		/// @brief ステート終了時に呼ばれる処理
		/// @param machine 呼び出し元のステートマシン
		virtual void OnExit(IStateMachine* machine) = 0;
		/// @brief 毎フレームの更新処理
		/// @param machine 呼び出し元のステートマシン
		/// @param dt 経過時間
		virtual void Update(IStateMachine* machine, float dt) = 0;
		/// @brief 衝突開始イベントを処理する
		/// @param machine 呼び出し元のステートマシン
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) {};
		/// @brief 衝突継続イベントを処理する
		/// @param machine 呼び出し元のステートマシン
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) {};
		/// @brief 衝突終了イベントを処理する
		/// @param machine 呼び出し元のステートマシン
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) {};
		/// @brief トリガー領域への侵入イベントを処理する
		/// @param machine 呼び出し元のステートマシン
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) {};
		/// @brief トリガー領域での継続イベントを処理する
		/// @param machine 呼び出し元のステートマシン
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnTriggerStay(IStateMachine* machine, const HitInfo& hit_info) {};
		/// @brief トリガー領域からの退出イベントを処理する
		/// @param machine 呼び出し元のステートマシン
		/// @param hit_info 衝突に関する詳細情報
		virtual void OnTriggerExit(IStateMachine* machine, const HitInfo& hit_info) {};
		/// @brief ステート名を取得する
		/// @return ステート名を示す文字列
		const std::string& GetName() const { return name; }
		/// @brief ステート名を設定する
		/// @param name_ 設定するステート名
		void SetName(const std::string_view name_) { name = name_; }
		/// @brief デバッグ用の描画処理を行う
		virtual void DebugDraw() {};
		/// @brief 登録された遷移要求を評価して遷移先を取得する
		/// @return 遷移先のステート名
		std::string GetChangeRequest();
		/// @brief 遷移要求を登録する
		/// @param state_name 遷移先のステート名
		/// @param func 遷移条件を判定する関数オブジェクト
		/// @param priority 優先度
		void RegisterChangeRequest(const std::string& state_name, std::function<bool()> func, u8 priority = 0);

		/// @brief 指定したステートへの遷移が可能か判定する
		/// @param state_name 判定対象の遷移先ステート名
		/// @return 遷移可能であればtrueを返す
		virtual bool CanTransitTo(const std::string& state_name) { return true; }
	private:
		GameObject* owner = nullptr; /// @brief 所有者となるゲームオブジェクトへのポインタ
		std::string name; /// @brief ステートの名前
		/// @brief 遷移要求を管理するための内部構造体
		struct FunctionRequest {
			u8 priority; /// @brief 判定の優先度
			std::string state_name; /// @brief 遷移先のステート名
			std::function<bool()> function = nullptr; /// @brief 遷移条件を判定する関数
		};
		std::vector <SafeUniquePtr<FunctionRequest>> change_requests; /// @brief 登録された遷移要求のリスト
	};
}