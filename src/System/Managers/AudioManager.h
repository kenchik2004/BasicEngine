//---------------------------------------------------------------------------
//! @file   AudioManager.h
//! @brief  オーディオリソースの非同期ロード・キャッシュ管理を行うマネージャー
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------
//! @class AudioManager
//! @brief オーディオリソースの非同期ロードとキャッシュを管理するクラス
//---------------------------------------------------------------------
class AudioManager
{
	AudioManager() = delete;
	AudioManager(const AudioManager&) = delete;
	~AudioManager() = default;
public:
	static inline std::atomic<int> loading_count = 0;	//!< 非同期ロード中のオーディオ数

	//---------------------------------------------------------------------
	//! @struct IndexAndHandle
	//! @brief  キャッシュ配列のインデックスとオーディオハンドルのペア
	//---------------------------------------------------------------------
	struct IndexAndHandle {
		int index = -1;	//!< キャッシュ配列のインデックス
		int handle = -1;	//!< オーディオハンドル
	};

	//---------------------------------------------------------------------
	//! @struct PtrToCacheAndAudioData
	//! @brief  非同期ロードスレッドに渡すキャッシュポインタとオーディオデータ
	//---------------------------------------------------------------------
	struct PtrToCacheAndAudioData {
		std::vector<SafeUniquePtr<AudioSource>>* cache;									 //!< キャッシュ配列へのポインタ
		std::unordered_map<std::string, IndexAndHandle>* name_map;						 //!< 名前→インデックスマップへのポインタ
		std::unordered_map<std::string, IndexAndHandle>* path_map;						 //!< パス→インデックスマップへのポインタ
		SafeUniquePtr<AudioSource> audio_source;										 //!< ロードしたオーディオソース
	};
	static std::vector<SafeUniquePtr<AudioSource>> cache;								 //!< ロード済みオーディオのキャッシュ
	static std::unordered_map<std::string, IndexAndHandle> names;						 //!< 名前→キャッシュエントリマップ
	static std::unordered_map<std::string, IndexAndHandle> paths;						 //!< パス→キャッシュエントリマップ

	//! @brief オーディオを非同期でロードする
	static void Load(std::string_view path, std::string_view name, bool use_3d = false);
	//! @brief 名前を指定してオーディオクリップを複製する
	static SafeSharedPtr<AudioClip> CloneByName(std::string_view name, std::string_view new_name = "");
	//! @brief パスを指定してオーディオクリップを複製する
	static SafeSharedPtr<AudioClip> CloneByPath(std::string_view path, std::string_view new_name = "");
	//! @brief 現在の非同期ロード中オーディオ数を取得する
	static inline const int GetLoadingCount() { return loading_count; }
	//! @brief 初期化処理
	static void Init();
	//! @brief 終了処理
	static void Exit();
};
