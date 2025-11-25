//----------------------------------------------------------------------------
//!	@file	Material.h
//!	@brief	マテリアルクラスの定義。シェーダーとテクスチャを管理し、描画に使用する。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#pragma once

//----------------------------------------------------
// @brief マテリアルを表すクラス。シェーダーとテクスチャを管理。
//----------------------------------------------------
class Material
{
	friend class MaterialManager; // MaterialManager クラスが private メンバーにアクセスできるようにする。
public:
	//----------------------------------------------------
	// @brief コンストラクタ。
	//----------------------------------------------------
	Material();
	//----------------------------------------------------
	// @brief デストラクタ。
	//----------------------------------------------------
	~Material();
	//----------------------------------------------------
	// @brief テクスチャタイプを表す列挙型。
	//----------------------------------------------------
	enum class TextureType :u32 {
		Diffuse, // ディフューズテクスチャ
		Normal, // ノーマルテクスチャ
		Specular, // スペキュラーテクスチャ
		Roughness, // ラフネステクスチャ
		Metalic, // メタリックテクスチャ
		Emission, // エミッシブテクスチャ
		Max, // 最大値
	};
private:
	ShaderPs* pixel_shader; // ピクセルシェーダー
	ShaderPs* pixel_shader_gbuffer; // ピクセルシェーダー(Gbuffer用)
	ShaderVs* vertex_shader; // 頂点シェーダー
	std::array<SafeSharedPtr<Texture>, static_cast<u32>(TextureType::Max)> textures; // テクスチャ配列
	std::string name; // マテリアル名
	static inline u32 instance_count = 0; // インスタンス数
public:
	//----------------------------------------------------
	// @brief ピクセルシェーダーを設定する。
	// @param gbuffer Gbuffer用シェーダーかどうか。
	// @param override_ps 設定するピクセルシェーダー。
	//----------------------------------------------------
	void SetShaderPs(ShaderPs* override_ps,bool gbuffer=false);
	//----------------------------------------------------
	// @brief 頂点シェーダーを設定する。
	// @param override_vs 設定する頂点シェーダー。
	//----------------------------------------------------
	void SetShaderVs(ShaderVs* override_vs);
	//----------------------------------------------------
	// @brief テクスチャを設定する。
	// @param texture 設定するテクスチャ。
	// @param texture_type テクスチャのタイプ。
	//----------------------------------------------------
	void SetTexture(SafeSharedPtr<Texture> texture, TextureType texture_type);
	//----------------------------------------------------
	// @brief 指定したタイプのテクスチャを取得する。
	// @param type 取得するテクスチャのタイプ。
	// @return テクスチャのポインタ。
	//----------------------------------------------------
	inline SafeSharedPtr<Texture> GetTexture(TextureType type) {
		if (type >= TextureType::Max)
			return nullptr;
		return textures[static_cast<u32>(type)];
	}
	//----------------------------------------------------
	// @brief ピクセルシェーダーを取得する。
	// @return ピクセルシェーダー。
	//----------------------------------------------------
	ShaderPs* GetPixelShader() { return pixel_shader; }

	//----------------------------------------------------
	// @brief ピクセルシェーダー(Gbuffer用)を取得する。
	// @return Gbuffer用ピクセルシェーダー
	//----------------------------------------------------
	ShaderPs* GetGbufferPixelShader() { return pixel_shader_gbuffer; }


	//----------------------------------------------------
	// @brief 頂点シェーダーを取得する。
	// @return 頂点シェーダー。
	//----------------------------------------------------
	ShaderVs* GetVertexShader() { return vertex_shader; }
};

