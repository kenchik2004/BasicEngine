#include "precompile.h"
#include "Exception.h"
#include "assert.h"
#include <codecvt>

Exception::Exception(const char* main_message, const char* file_name, int line, const char* func_name)
{
	message = "ファイル名: ";
	message += file_name;
	message += "\n行: ";
	message += std::to_string(line);
	message += "\n関数名: ";
	message += func_name;
	message += "\nで例外スロー: \n";
	message += main_message;
}

void Exception::Show()
{
	std::wstring wstr = Str2Wstr(message);

#ifndef NDEBUG
	_wassert(wstr.c_str(), __FILEW__, __LINE__);
#else
	_ASSERT(false, "ERROR!!");
#endif


}

NullptrException::NullptrException(const char* integer_name, const char* file_name, int line, const char* func_name)
{
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

OutOfRangeException::OutOfRangeException(int index, int array_size, const char* array_name, const char* file_name, int line, const char* func_name)
{
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
