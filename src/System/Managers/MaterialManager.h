//---------------------------------------------------------------------------
//! @file   MaterialManager.h
//! @brief  マテリアル・シェーダーリソースの生成・取得・解放を管理するマネージャー
//---------------------------------------------------------------------------
#pragma once
#include "System/Utils/Material.h"

//---------------------------------------------------------------------
//! @class MaterialManager
//! @brief マテリアルおよびシェーダーリソースを一元管理するクラス
//---------------------------------------------------------------------
class MaterialManager
{
public:
	static inline std::unordered_map<std::string, SafeUniquePtr<Material>> materials;			//!< 登録済みマテリアルのマップ
	static inline std::unordered_map<std::string, SafeUniquePtr<ShaderPs>> pixel_shaders;		//!< 登録済みピクセルシェーダーのマップ
	static inline std::unordered_map<std::string, SafeUniquePtr<ShaderVs>> vertex_shaders;		//!< 登録済み頂点シェーダーのマップ

	//! @brief マテリアルを新規作成する
	static Material* CreateMaterial(std::string_view name);
	//! @brief 名前を指定してマテリアルを取得する
	static Material* GetMaterial(std::string_view name);
	//! @brief ピクセルシェーダーをロードする
	static ShaderPs* LoadPixelShader(std::string_view path, std::string_view name);
	//! @brief 頂点シェーダーをロードする
	static ShaderVs* LoadVertexShader(std::string_view path, std::string_view name, int variant);
	//! @brief デフォルト2Dマテリアルを取得する
	static Material* GetDefaultMat2D();
	//! @brief デフォルト3Dマテリアルを取得する
	static Material* GetDefaultMat3D();
	//! @brief デフォルトGBufferマテリアルを取得する
	static Material* GetDefaultMatGBuffer();

	//! @brief 初期化処理
	static void Init();
	//! @brief シェーダーの再読み込みを行う
	static void UpdateShaders();
	//! @brief リソースを解放する
	static void Release();
private:
	static inline SafeUniquePtr<ShaderPs> default_pixel_shader;			//!< デフォルトピクセルシェーダー
	static inline SafeUniquePtr<ShaderPs> default_pixel_shader_gbuffer;	//!< デフォルトGBuffer用ピクセルシェーダー
	static inline SafeUniquePtr<ShaderPs> default_pixel_shader_2d;			//!< デフォルト2D用ピクセルシェーダー
	static inline SafeUniquePtr<ShaderVs> default_vertex_shader;			//!< デフォルト頂点シェーダー
	static inline SafeSharedPtr<Texture> null_white;						//!< ホワイトのデフォルトテクスチャ
	static inline SafeSharedPtr<Texture> null_black;						//!< ブラックのデフォルトテクスチャ
	static inline SafeSharedPtr<Texture> null_normal;						//!< デフォルトノーマルマップテクスチャ
};
