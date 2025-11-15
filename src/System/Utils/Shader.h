//----------------------------------------------------------------------------
//!	@file	Shader.h
//!	@brief	シェーダー関連のクラス定義。シェーダーの読み込み、コンパイル、バリエーション管理を提供。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#pragma once

#include <tuple>

//----------------------------------------------------
// @brief シェーダー基底クラス。
//----------------------------------------------------
class Shader
{
public:
	//----------------------------------------------------
	// @brief デストラクタ。
	//----------------------------------------------------
	virtual ~Shader();

	//----------------------------------------------------
	// @brief 作成。
	// @param path ファイルパス。
	// @param dxlib_shader_type [DxLib] シェーダーの種類(DX_SHADERTYPE_VERTEXなど)。
	// @param variant_count シェーダーバリエーション数(default:0)。
	//----------------------------------------------------
	Shader(std::string_view path, u32 dxlib_shader_type, u32 variant_count = 0);

	//----------------------------------------------------
	// @brief [DxLib] シェーダーハンドルを取得。
	//----------------------------------------------------
	operator int() const;

	//----------------------------------------------------
	// @brief [DxLib] シェーダーハンドルを取得 (バリエーション指定)。
	// @param variant_index バリエーションインデックス。
	//----------------------------------------------------
	int variant(u32 variant_index) const;

	//----------------------------------------------------
	// @brief [DxLib] シェーダーバイトコードを取得 (バリエーション指定)。
	// @param variant_index バリエーションインデックス。
	//----------------------------------------------------
	std::tuple<const void*, size_t> shader_bytecode(u32 variant_index = 0) const;

	//----------------------------------------------------
	// @brief ファイルパスを取得。
	//----------------------------------------------------
	const std::wstring& path() const;

	//----------------------------------------------------
	// @brief ファイル監視を更新。
	//----------------------------------------------------
	static void updateFileWatcher();

private:
	struct Impl;
	std::shared_ptr<Impl> impl_;
};

//----------------------------------------------------
// @brief 頂点シェーダー。
//----------------------------------------------------
class ShaderVs final : public Shader
{
public:
	ShaderVs(std::string_view path, u32 variant_count = 0)
		: Shader(path, DX_SHADERTYPE_VERTEX, variant_count)
	{
	}

private:
};

//----------------------------------------------------
// @brief ピクセルシェーダー。
//----------------------------------------------------
class ShaderPs final : public Shader
{
public:
	ShaderPs(std::string_view path, u32 variant_count = 0)
		: Shader(path, DX_SHADERTYPE_PIXEL, variant_count)
	{
	}
};

//----------------------------------------------------
// @brief ジオメトリシェーダー。
//----------------------------------------------------
class ShaderGs final : public Shader
{
public:
	ShaderGs(std::string_view path, u32 variant_count = 0)
		: Shader(path, DX_SHADERTYPE_GEOMETRY, variant_count)
	{
	}
};

//----------------------------------------------------
// @brief コンピュートシェーダー。
//----------------------------------------------------
class ShaderCs final :public Shader
{
public:
	ShaderCs(std::string_view path, u32 variant_count = 0)
		:Shader(path, DX_SHADERTYPE_COMPUTE, variant_count)
	{
	}
};