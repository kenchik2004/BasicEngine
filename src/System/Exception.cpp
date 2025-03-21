#include "precompile.h"
#include "Exception.h"
#include "assert.h"
#include <codecvt>

Exception::Exception(const char* main_message, const char* file_name, int line, const char* func_name)
{
	message = "�t�@�C����: ";
	message += file_name;
	message += "\n�s: ";
	message += std::to_string(line);
	message += "\n�֐���: ";
	message += func_name;
	message += "\n�ŗ�O�X���[: \n";
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
	message = "�t�@�C����: ";
	message += file_name;
	message += "\n�s: ";
	message += std::to_string(line);
	message += "\n�֐���: ";
	message += func_name;
	message += "\n�ŗ�O�X���[: \n";
	message += integer_name;
	message += " ��nullptr�ł����B\n";

}

OutOfRangeException::OutOfRangeException(int index, int array_size, const char* array_name, const char* file_name, int line, const char* func_name)
{
	message = "�t�@�C����: ";
	message += file_name;
	message += "\n�s: ";
	message += std::to_string(line);
	message += "\n�֐���: ";
	message += func_name;
	message += "\n�ŗ�O�X���[: \n";
	message += std::to_string(index);
	message += "�́A";
	message += array_name;
	message += "�̃T�C�Y [";
	message += std::to_string(array_size);
	message += "] �𒴂��Ă��܂��B\n";
}

MemoryLeakException::MemoryLeakException(const char* integer_name, const char* file_name, int line, const char* func_name)
{
	message = "�t�@�C����: ";
	message += file_name;
	message += "\n�s: ";
	message += std::to_string(line);
	message += "\n�֐���: ";
	message += func_name;
	message += "\n�ŗ�O�X���[: \n";
	message += integer_name;
	message += "������ł��܂���ł����B�z�Q�Ƃ��N�����Ă���\��������܂��B\n �ϐ���std::weak_ptr�ŕێ��A�������������std::shared_ptr��nullptr�����Ȃǂ������Ă݂Ă��������B";
}
