//---------------------------------------------------------------------------
//! @file   ModelManager.h
//! @brief  3Dモデル・アニメーションの非同期ロード・キャッシュ管理を行うマネージャー
//---------------------------------------------------------------------------
#pragma once


//---------------------------------------------------------------------
//! @class ModelManager
//! @brief 3Dモデルおよびアニメーションリソースの非同期ロードとキャッシュを管理するクラス
//---------------------------------------------------------------------
class ModelManager
{
	ModelManager() = delete;
	ModelManager(const ModelManager&) = delete;
	~ModelManager() = default;
public:
	static std::vector<SafeUniquePtr<ModelSource>> model_chache;	//!< ロード済みモデルソースのキャッシュ
	static std::vector<SafeUniquePtr<AnimSource>> anim_chache;		//!< ロード済みアニメーションソースのキャッシュ

	//---------------------------------------------------------------------
	//! @struct IndexAndHandle
	//! @brief  キャッシュ配列のインデックスとリソースハンドルのペア
	//---------------------------------------------------------------------
	struct IndexAndHandle {
		int index = -1;	//!< キャッシュ配列のインデックス
		int handle = -1;	//!< リソースハンドル
	};
	static inline std::atomic<int> loading_count = 0;	//!< 非同期ロード中のモデル数

	//---------------------------------------------------------------------
	//! @struct PtrToCacheAndModelData
	//! @brief  非同期ロードスレッドに渡すキャッシュポインタとモデル・アニメーションデータ
	//---------------------------------------------------------------------
	struct PtrToCacheAndModelData {
		std::vector<SafeUniquePtr<ModelSource>>* model_cache = nullptr;						 //!< モデルキャッシュ配列へのポインタ
		std::vector<SafeUniquePtr<AnimSource>>* anim_cache = nullptr;						 //!< アニメーションキャッシュ配列へのポインタ
		SafeUniquePtr<ModelSource> m_source;												 //!< ロードしたモデルソース
		SafeUniquePtr<AnimSource> a_source;													 //!< ロードしたアニメーションソース
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* m_name = nullptr;	 //!< モデル名→インデックスマップへのポインタ
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* m_path = nullptr;	 //!< モデルパス→インデックスマップへのポインタ
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* a_name = nullptr;	 //!< アニメーション名→インデックスマップへのポインタ
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* a_path = nullptr;	 //!< アニメーションパス→インデックスマップへのポインタ
	};
	static std::unordered_map<std::string, IndexAndHandle> m_name;	//!< モデル名→キャッシュエントリマップ
	static std::unordered_map<std::string, IndexAndHandle> m_path;	//!< モデルパス→キャッシュエントリマップ
	static std::unordered_map<std::string, IndexAndHandle> a_name;	//!< アニメーション名→キャッシュエントリマップ
	static std::unordered_map<std::string, IndexAndHandle> a_path;	//!< アニメーションパス→キャッシュエントリマップ

	//! @brief モデルとして非同期ロードする
	static void LoadAsModel(std::string_view path, std::string_view name);
	//! @brief アニメーションとして非同期ロードする
	static void LoadAsAnimation(std::string_view path, std::string_view name);
	//! @brief 名前を指定してモデルを複製する
	static SafeSharedPtr<Model> CloneModelByName(std::string_view name, std::string_view new_name = "");
	//! @brief パスを指定してモデルを複製する
	static SafeSharedPtr<Model> CloneModelByPath(std::string_view path, std::string_view new_name = "");
	//! @brief 名前を指定してアニメーションを複製する
	static SafeSharedPtr<Animation> CloneAnimByName(std::string_view name, int index = 0, std::string_view new_name = "");
	//! @brief パスを指定してアニメーションを複製する
	static SafeSharedPtr<Animation> CloneAnimByPath(std::string_view path, int index = 0, std::string_view new_name = "");
	//! @brief 現在の非同期ロード中モデル数を取得する
	static inline const int GetLoadingCount() { return loading_count; }
	//! @brief 初期化処理
	static void Init();
	//! @brief 終了処理
	static void Exit();



};
