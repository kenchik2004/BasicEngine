//----------------------------------------------------------------------------
//!	@file	Exception.h
//!	@brief	例外処理関連のクラス定義。様々な例外クラスを定義し、エラーハンドリングを提供。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#pragma once
#define DEFAULT_EXCEPTION_PARAM __FILE__,__LINE__,__FUNCTION__ //< @brief デフォルトの例外パラメータマクロ。ファイル名、行番号、関数名を渡す。
#define INTEGER_NAME(integer) #integer //< @brief 整数を文字列に変換するマクロ。 

//----------------------------------------------------
// @brief デフォルトの例外検知用クラス。
//----------------------------------------------------
class Exception
{
public:
	//----------------------------------------------------
	// @brief コンストラクタ。マクロ DEFAULT_EXCEPTION_PARAMを使用して初期化ができる。
	// 引数：
	//	メインメッセージの内容
	//	例外が発生したファイルの名前
	//	例外が発生した行番号 
	//	例外が発生した関数名
	//----------------------------------------------------
	Exception(const char* main_message, const char* file_name, int line, const char* func_name);
	//----------------------------------------------------
	// @brief 例外を表示する。
	//----------------------------------------------------
	void Show();
protected:
	bool is_assert = false; //< @brief アサートとして扱うかどうかのフラグ。 
	Exception() {} //< @brief 保護されたデフォルトコンストラクタ。 
	std::string message = ""; //< @brief 最終的に出力するメッセージ。 
};

//----------------------------------------------------
// @brief nullptrへのアクセス検知用例外クラス。
//----------------------------------------------------
class NullptrException :public Exception {
public:
	NullptrException() = delete;		//< @brief 規定コンストラクタの削除。
	//----------------------------------------------------
	// @brief コンストラクタ。マクロ　INTEGER_NAME と DEFAULT_EXCEPTION_PARAMを使用して初期化ができる。
	// 引数：
	//	アクセスしようとした変数名
	//	例外が発生したファイルの名前
	//	例外が発生した行番号 
	//	例外が発生した関数名
	//----------------------------------------------------
	NullptrException(const char* integer_name, const char* file_name, int line, const char* func_name);
	//----------------------------------------------------
	// @brief メッセージを指定して初期化するコンストラクタ。
	// @param message_ 例外メッセージ。
	//----------------------------------------------------
	NullptrException(const char* message_);
};

//----------------------------------------------------
// @brief 配列外アクセス検知用例外クラス。
//----------------------------------------------------
class OutOfRangeException :public Exception {
public:
	OutOfRangeException() = delete;		//< @brief 規定コンストラクタの削除。 
	//----------------------------------------------------
	// @brief コンストラクタ。マクロ　INTEGER_NAME と DEFAULT_EXCEPTION_PARAMを使用して初期化ができる。
	// 引数：
	//	アクセスしようとしたインデックス
	//	アクセスしようとした配列のサイズ
	//	アクセスしようとした配列の名前
	//	例外が発生したファイルの名前
	//	例外が発生した行番号 
	//	例外が発生した関数名
	//----------------------------------------------------
	OutOfRangeException(int index, int array_size, const char* array_name, const char* file_name, int line, const char* func_name);
};
//----------------------------------------------------
// @brief メモリリーク検知用例外クラス。
//----------------------------------------------------
class MemoryLeakException :public Exception {
public:
	MemoryLeakException() = delete;		//< @brief 規定コンストラクタの削除。 
	//----------------------------------------------------
	// @brief コンストラクタ。マクロ　INTEGER_NAME と DEFAULT_EXCEPTION_PARAMを使用して初期化ができる。
	// 引数：
	//	解放できなかった変数の名前(またはオブジェクト・コンポーネント名)
	//	例外が発生したファイルの名前
	//	例外が発生した行番号 
	//	例外が発生した関数名
	//----------------------------------------------------
	MemoryLeakException(const char* integer_name, const char* file_name, int line, const char* func_name);
};

