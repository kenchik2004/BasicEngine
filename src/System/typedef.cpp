﻿#include "precompile.h"


//通常stringをワイドstringに変換
std::wstring Str2Wstr(std::string in)
{
	// 変換に必要なサイズを取得
	int size_needed = MultiByteToWideChar(CP_ACP, 0, in.c_str(), static_cast<int>(in.size()), NULL, 0);
	// 変換先のwchar_t配列を作成
	std::wstring out(size_needed, 0);
	// 変換を実行
	MultiByteToWideChar(CP_ACP, 0, in.c_str(), static_cast<int>(in.size()), &out[0], size_needed);
	return out;
}

//ワイドstringを通常stringに変換
std::string WStr2Str(std::wstring in)
{

	// 変換に必要なサイズを取得
	int size_needed = WideCharToMultiByte(CP_ACP, 0, in.c_str(), static_cast<int>(in.size()), NULL, 0, NULL, NULL);
	// 変換先のwchar_t配列を作成
	std::string out(size_needed, 0);
	// 変換を実行
	WideCharToMultiByte(CP_ACP, 0, in.c_str(), static_cast<int>(in.size()), &out[0], size_needed, NULL, NULL);
	return out;
}

std::string_view TypeInfo::ClassName() const
{
	return class_name;
}


size_t TypeInfo::ClassSize() const
{
	return class_size;
}

const TypeInfo* TypeInfo::Parent() const
{
	return parent;
}

const TypeInfo* TypeInfo::Child() const
{
	return child;
}

const TypeInfo* TypeInfo::Sibling() const
{
	return sibling;
}

