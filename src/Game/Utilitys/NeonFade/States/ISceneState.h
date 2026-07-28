// ISceneState.h
//---------------------------------------------------------------------------
//! @file   ISceneState.h
//! @brief  シーン状態の基底インタフェースの定義
//---------------------------------------------------------------------------
#pragma once
namespace NeonFade {

	class ISceneStateMachine;
	/// @brief シーンステートマシン内で使用する各ステートの基底クラス
	class ISceneState
	{
	public:
		/// @brief デストラクタ
		virtual ~ISceneState() = default;
		/// @brief コンストラクタ
		/// @param owner_scene_ 所有者となるシーンのポインタ
		ISceneState(Scene* owner_scene_);
		/// @brief ステート開始時に呼ばれる処理
		/// @param machine 呼び出し元のステートマシン
		virtual void OnEnter(ISceneStateMachine* machine) = 0;
		/// @brief ステート終了時に呼ばれる処理
		/// @param machine 呼び出し元のステートマシン
		virtual void OnExit(ISceneStateMachine* machine) = 0;
		/// @brief 毎フレームの更新処理
		/// @param machine 呼び出し元のステートマシン
		/// @param dt 経過時間
		virtual void Update(ISceneStateMachine* machine, float dt) = 0;
		/// @brief ステート名を取得する
		/// @return ステート名を示す文字列
		const std::string& GetName() { return name; }
		/// @brief ステート名を設定する
		/// @param name_ 設定するステート名
		void SetName(const std::string_view name_) { name = name_; }
		/// @brief デバッグ用の描画処理
		virtual void DebugDraw() {};
		/// @brief 登録された遷移要求を評価して遷移先を取得する
		/// @return 遷移先のステート名
		std::string GetChangeRequest();
		/// @brief 遷移要求を登録する
		/// @param state_name 遷移先のステート名
		/// @param func 遷移条件を判定する関数オブジェクト
		/// @param priority 優先度
		void RegisterChangeRequest(const std::string& state_name, std::function<bool()>& func, u8 priority = 0);
	private:
		Scene* owner_scene = nullptr; /// @brief 所有者となるシーンのポインタ
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