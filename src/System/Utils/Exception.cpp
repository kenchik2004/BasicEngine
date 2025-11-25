//----------------------------------------------------------------------------
//!	@file	Exception.cpp
//!	@brief	例外処理の実装。Exception クラスのメソッドを実装し、エラーメッセージの生成と表示を行う。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "assert.h"
#include <codecvt>
#include <deque>

//----------------------------------------------------
// @brief ログを出力する関数。ログを履歴として保持し、必要に応じて表示。
// @param log 出力するログメッセージ。
//----------------------------------------------------
void Log(std::string log) {
	static std::deque<std::string> log_lines; /**< @brief ログ行を保持する静的キュー。 */

	log_lines.emplace_back(log); /**< @brief ログをキューに追加。 */
	if (log_lines.size() > 1000) // 履歴保持数制限
		log_lines.pop_front(); /**< @brief 履歴が1000を超えたら古いログを削除。 */
#if 0
	//	ImGui::Begin("console");
	for (auto& log : log_lines) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), ShiftJISToUTF8(log).c_str());
	}
	//	ImGui::End();
#endif
}

//----------------------------------------------------
// @brief Exception のコンストラクタ。
//----------------------------------------------------
Exception::Exception(const char* main_message, const char* file_name, int line, const char* func_name)
{
	// メッセージを構築
	message = "ファイル名: ";
	message += file_name;
	message += "\n行: ";
	message += std::to_string(line);
	message += "\n関数名: ";
	message += func_name;
	message += "\nで例外スロー: \n";
	message += main_message;
}

//----------------------------------------------------
// @brief 例外を表示する。
//----------------------------------------------------
void Exception::Show()
{
	// メッセージを表示
	printfDx(message.c_str());
	printfDx("\n");
	//#ifndef PACKAGE_BUILD
		//Log(message);
	//#endif
#ifndef NDEBUG
	std::wstring wstr = Str2Wstr(message); /**< @brief メッセージをワイド文字列に変換。 */
	if (is_assert)
		_wassert(wstr.c_str(), __FILEW__, __LINE__); /**< @brief アサートとして処理。 */
#else
	if (is_assert)
		_ASSERT(false, "ERROR!!"); /**< @brief リリースビルドでのアサート。 */
#endif
}

//----------------------------------------------------
// @brief NullptrException のコンストラクタ。
//----------------------------------------------------
NullptrException::NullptrException(const char* integer_name, const char* file_name, int line, const char* func_name)
{
	// メッセージを構築
	message = "ファイル名: ";
	message += file_name;
	message += "\n行: ";
	message += std::to_string(line);
	message += "\n関数名: ";
	message += func_name;
	message += "\nで例外スロー: \n";
	message += integer_name;
	message += " がnullptrでした。\n";
}

//----------------------------------------------------
// @brief NullptrException のメッセージ指定コンストラクタ。
// @param message_ 例外メッセージ。
//----------------------------------------------------
NullptrException::NullptrException(const char* message_)
{
	message += message_; /**< @brief メッセージを追加。 */
}

//----------------------------------------------------
// @brief OutOfRangeException のコンストラクタ。
//----------------------------------------------------
OutOfRangeException::OutOfRangeException(int index, int array_size, const char* array_name, const char* file_name, int line, const char* func_name)
{
	// メッセージを構築
	message = "ファイル名: ";
	message += file_name;
	message += "\n行: ";
	message += std::to_string(line);
	message += "\n関数名: ";
	message += func_name;
	message += "\nで例外スロー: \n";
	message += std::to_string(index);
	message += "は、";
	message += array_name;
	message += "のサイズ [";
	message += std::to_string(array_size);
	message += "] を超えています。\n";
}

//----------------------------------------------------
// @brief MemoryLeakException のコンストラクタ。
//----------------------------------------------------
MemoryLeakException::MemoryLeakException(const char* integer_name, const char* file_name, int line, const char* func_name)
{
	is_assert = true; /**< @brief アサートフラグをtrueに設定。 */
	// メッセージを構築
	message = "ファイル名: ";
	message += file_name;
	message += "\n行: ";
	message += std::to_string(line);
	message += "\n関数名: ";
	message += func_name;
	message += "\nで例外スロー: \n";
	message += integer_name;
	message += "が解放できませんでした。循環参照が起こっている可能性があります。\n 変数をstd::weak_ptrで保持、メモリ解放時にstd::shared_ptrにnullptrを代入などを試してみてください。";
}
