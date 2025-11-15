//----------------------------------------------------------------------------
//!	@file	FileWatcher.cpp
//!	@brief	ファイル変更監視の実装。FileWatcher クラスのメソッドを実装し、ファイル変更を検知してコールバックを呼び出す。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "FileWatcher.h"

//----------------------------------------------------
// @brief 初期化。
//----------------------------------------------------
bool FileWatcher::initialize(const wchar_t* path, std::function<void(const wchar_t*)> callback)
{
	callback_ = callback; //< @brief コールバック関数を設定

	// フルパスに変換
	{
		auto size = GetFullPathNameW(path, 0, nullptr, nullptr); //< @brief フルパスのサイズを取得。
		full_path_.resize(size); //< @brief フルパス用の文字列をリサイズ。 
		GetFullPathNameW(path, size, full_path_.data(), nullptr); //< @brief フルパスを取得。
	}
	// コンソール出力を日本語可能に
	setlocale(LC_ALL, ""); //< @brief ロケールを設定して日本語出力可能に。

	// 対象のディレクトリを監視用にオープンする。
	// 共有ディレクトリ使用可、対象フォルダを削除可
	// 非同期I/O使用
	handle_directory_ = CreateFileW(full_path_.c_str(),                                        // [in] 監視先パス
		FILE_LIST_DIRECTORY,                                       // [in] 要求アクセス
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,    // [in] 共有モード
		nullptr,                                                   // [in] セキュリティ属性
		OPEN_EXISTING,                                             // [in] フォルダ가存在する場合にのみ成功
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,         // [in] ReadDirectoryChangesW用
		nullptr);                                                  //
	if (handle_directory_ == INVALID_HANDLE_VALUE) {
		return false; //< @brief ハンドル作成失敗。
	}

	// 変更されたファイルのリストを記録するためのバッファ
	// 最初のReadDirectoryChangesWの通知から次のReadDirectoryChangesWまでの
	// 間に変更されたファイルの情報を格納できるだけのサイズが必要
	// バッファオーバーとしてもファイルに変更が発生したことは感知できるが、
	// なにが変更されたかは通知できない。
	buffer_.resize(64 * 1024); //< @brief バッファを64KBにリサイズ。 

	// 非同期I/Oの完了待機用 (手動リセットモード)
	// 変更通知のイベント発行とキャンセル完了のイベント発行の
	// 2つのイベントソースがあるためイベントの流れが予想できず
	// 自動リセットイベントにするのは危険。
	finish_event_ = CreateEvent(nullptr, true, false, nullptr); //< @brief 完了イベントを作成。 
	if (finish_event_ == nullptr) {
		return false; //< @brief イベント作成失敗。 
	}

	// イベントの手動リセット
	ResetEvent(finish_event_); //< @brief イベントをリセット。 

	// 非同期I/O
	overlapped_.hEvent = finish_event_; //< @brief オーバーラップにイベントを設定。

	// 監視を開始
	start(); //< @brief 監視を開始。 

	return true; //< @brief 初期化成功。
}

//----------------------------------------------------
// @brief 更新。
//----------------------------------------------------
void FileWatcher::update()
{
	// 変更通知チェック
	u32 result = WaitForSingleObject(finish_event_, 5); //< @brief イベントを5ms待機
	if (result == WAIT_TIMEOUT) {
		return; //< @brief タイムアウトなので何もしない。
	}

	// 非同期I/Oの結果を取得する
	DWORD size = 0;
	if (!GetOverlappedResult(handle_directory_, &overlapped_, &size, FALSE)) {
		// 結果取得に失敗した場合
		return; //< @brief 結果取得失敗
	}
	if (size == 0) {
		// 返却サイズが0ならばバッファオーバーを示す
		return; /**< @brief バッファオーバー。 */
	}

	// 最初のエントリに位置付ける
	auto data = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer_.data()); //< @brief バッファからデータ取得

	// エントリの末尾まで繰り返す
	for (;;) {
		// アクションタイプによって処理。現時点では特殊な処理は無し。
		switch (data->Action) {
		case FILE_ACTION_ADDED:
			break; //< @brief ファイル追加
		case FILE_ACTION_REMOVED:
			break; //< @brief ファイル削除 
		case FILE_ACTION_MODIFIED:
			break; //< @brief ファイル変更 
		case FILE_ACTION_RENAMED_OLD_NAME:
			break; //< @brief リネーム旧名 
		case FILE_ACTION_RENAMED_NEW_NAME:
			break; //< @brief リネーム新名 
		default:
			break; //< @brief その他
		}

		// ファイル名はnull終端されていないため長さから終端を付与
		u32          byte_size = data->FileNameLength;    // 文字数ではなくバイト数
		std::wstring file_name;
		file_name.resize(byte_size / sizeof(wchar_t)); //< @brief ファイル名用のワイド文字列をリサイズ。 

		memcpy(file_name.data(), data->FileName, byte_size); //< @brief ファイル名をコピー。

		// バックスラッシュをスラッシュに変換
		for (auto& c : file_name) {
			if (c == L'\\') {
				c = L'/'; //< @brief バックスラッシュをスラッシュに変換。
			}
		}

		// アクションと対象ファイルを処理
		callback_(file_name.data()); //< @brief コールバックを呼び出し。

		if (data->NextEntryOffset == 0) {
			start();    // 監視を再開

			break; //< @brief ループ終了。
		}
		// 次のエントリの位置まで移動する (現在アドレスからの相対バイト数)
		data = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<std::intptr_t>(data) + data->NextEntryOffset); //< @brief 次のエントリへ移動。
	}
}

//----------------------------------------------------
// @brief デストラクタ。
//----------------------------------------------------
FileWatcher::~FileWatcher()
{
	// 途中終了するなら非同期I/Oも中止
	// Overlapped構造体をシステムが使わなくなるまで待機する必要がある
	CancelIo(handle_directory_); //< @brief 非同期I/Oをキャンセル。 
	if (finish_event_ != INVALID_HANDLE_VALUE) {
		WaitForSingleObject(finish_event_, INFINITE); //< @brief イベントを無限待機。 
	}

	// ハンドルの解放
	CloseHandle(finish_event_); //< @brief イベントハンドルを閉じる。 
	CloseHandle(handle_directory_); //< @brief ディレクトリハンドルを閉じる。 
}

//----------------------------------------------------
// @brief ファイル監視を開始。
//----------------------------------------------------
bool FileWatcher::start()
{
	// 監視条件 (FindFirstChangeNotificationと同じ)
	constexpr u32 filter = FILE_NOTIFY_CHANGE_FILE_NAME |     // ファイル名の変更
		FILE_NOTIFY_CHANGE_DIR_NAME |      // ディレクトリ名の変更
		FILE_NOTIFY_CHANGE_ATTRIBUTES |    // 属性の変更
		FILE_NOTIFY_CHANGE_SIZE |          // サイズの変更
		FILE_NOTIFY_CHANGE_LAST_WRITE;     // 最終書き込み日時の変更

	// 変更を監視する
	// 初回呼び出し時にシステムが指定サイズでバッファを確保し、そこに変更を記録する
	// 完了通知後もシステムは変更を追跡しており、後続のReadDirectoryChangeWの
	// 呼び出しで、前回通知後からの変更をまとめて受け取ることができる
	// バッファがあふれた場合はサイズ0で応答が返される
	if (!ReadDirectoryChangesW(handle_directory_,                     // 対象ディレクトリ
		buffer_.data(),                        // 通知を格納するバッファ
		static_cast<DWORD>(buffer_.size()),    // バッファサイズ
		true,                                  // サブディレクトリを対象にするかどうか
		filter,                                // 変更通知を受け取るフィルタ
		nullptr,                               // (結果サイズ, 非同期なので未使用)
		&overlapped_,                          // 非同期I/Oバッファ
		nullptr                                // (完了ルーチン, 未使用)
	)) {
		// 開始できなかった場合のエラー
		return false; //< @brief 監視開始失敗。
	}
	return true; //< @brief 監視開始成功。
}
