#include "precompile.h"

std::wstring Str2Wstr(std::string in)
{
	// �ϊ��ɕK�v�ȃT�C�Y���擾
	int size_needed = MultiByteToWideChar(CP_OEMCP, 0, in.c_str(), static_cast<int>(in.size()), NULL, 0);
	// �ϊ����wchar_t�z����쐬
	std::wstring out(size_needed, 0);
	// �ϊ������s
	MultiByteToWideChar(CP_OEMCP, 0, in.c_str(), static_cast<int>(in.size()), &out[0], size_needed);
	return out;
}

std::string WStr2Str(std::wstring in)
{

	// �ϊ��ɕK�v�ȃT�C�Y���擾
	int size_needed = WideCharToMultiByte(CP_OEMCP, 0, in.c_str(), static_cast<int>(in.size()), NULL, 0, NULL, NULL);
	// �ϊ����wchar_t�z����쐬
	std::string out(size_needed, 0);
	// �ϊ������s
	WideCharToMultiByte(CP_OEMCP, 0, in.c_str(), static_cast<int>(in.size()), &out[0], size_needed, NULL, NULL);
	return out;
}
