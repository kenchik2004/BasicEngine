#include "precompile.h"


//�ʏ�string�����C�hstring�ɕϊ�
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

//���C�hstring��ʏ�string�ɕϊ�
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

const char* TypeInfo::ClassName() const
{
	return class_name.c_str();
}

size_t TypeInfo::ClassSize() const
{
	return class_size;
}

const TypeInfo* TypeInfo::Parent() const
{
	return parent;
}

TypeInfo TypeInfo::Root = TypeInfo("root", sizeof(TypeInfo), nullptr);
