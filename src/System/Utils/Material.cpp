//----------------------------------------------------------------------------
//!	@file	Material.cpp
//!	@brief	マテリアルクラスの実装。Material クラスのメソッドを実装。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "Material.h"
#include "System/Managers/MaterialManager.h"

//----------------------------------------------------
// @brief コンストラクタ。
//----------------------------------------------------
Material::Material()
{
	instance_count++; // インスタンス数をインクリメント
}

//----------------------------------------------------
// @brief デストラクタ。
//----------------------------------------------------
Material::~Material()
{
	instance_count--; // インスタンス数をデクリメント
}

//----------------------------------------------------
// @brief ピクセルシェーダーを設定する。
// @param gbuffer Gbuffer用シェーダーかどうか。
// @param override_ps 設定するピクセルシェーダー。
//----------------------------------------------------
void Material::SetShaderPs(ShaderPs* override_ps, bool gbuffer)
{
	if (gbuffer)
		pixel_shader_gbuffer = override_ps; // Gbuffer用ピクセルシェーダーを設定
	else
		pixel_shader = override_ps; // ピクセルシェーダーを設定
}


//----------------------------------------------------
// @brief 頂点シェーダーを設定する。
// @param override_vs 設定する頂点シェーダー。
//----------------------------------------------------
void Material::SetShaderVs(ShaderVs* override_vs)
{
	vertex_shader = override_vs; // 頂点シェーダーを設定
}

//----------------------------------------------------
// @brief テクスチャを設定する。
// @param texture 設定するテクスチャ。
// @param texture_type テクスチャのタイプ。
//----------------------------------------------------
void Material::SetTexture(SafeSharedPtr<Texture> texture, TextureType texture_type)
{
	if (!texture)
		texture = MaterialManager::GetDefaultMat3D()->GetTexture(texture_type); // デフォルトテクスチャを取得
	textures[static_cast<u32>(texture_type)] = texture; // テクスチャを設定
}





