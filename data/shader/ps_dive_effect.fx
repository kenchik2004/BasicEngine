//----------------------------------------------------------------------------
//!	@file	ps_dive_effect.fx
//!	@brief	MV1モデルピクセルシェーダー / GBuffer出力 (タイトル飛び込み時のエフェクト用)
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "shadow.h.fx"
#include "gbuffer.h.fx"

// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
    float4 position_ : SV_Position; //!< 座標       (スクリーン空間)
    float4 curr_position_ : CURR_POSITION; //!< 現在の座標 (スクリーン空間)
    float3 world_position_ : WORLD_POSITION; //!< ワールド座標
    float3 normal_ : NORMAL0; //!< 法線
    float4 diffuse_ : COLOR0; //!< Diffuseカラー
    float2 uv0_ : TEXCOORD0; //!< テクスチャ座標
    float4 prev_position_ : PREV_POSITION; //!< 1フレーム前の座標 (スクリーン空間) ※末尾に追加されているため注意
};

typedef VS_OUTPUT_MODEL PS_INPUT_MODEL;



//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_MRT main(PS_INPUT_MODEL input)
{
    float2 uv = input.uv0_;
    
    // 時間でUVをスクロールさせる
    uv.x += BE_Default.time*0.8;

    float3 N = normalize(input.normal_); // 法線

	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);
    textureColor = saturate(textureColor);
    

	// アルファテスト
    if (textureColor.a < 0.5)
        discard;
    
    // sRGBからリニアへの変換
    // モデルのDiffuseテクスチャはsRGBで作られていることが多いため、リニアに変換する必要がある。
    // リニアワークフローについて:
    // https://ny-program.hatenablog.com/entry/2021/08/28/144529
    
    textureColor.rgb = pow(textureColor.rgb, 2.2);
    
    float3 albedo = textureColor.rgb * input.diffuse_.rgb;
    albedo = LinearSRGB2ACEScg(albedo);
#ifdef DEBUG_CASCADE
    float shadow = GetShadowWithCascadeColor(input.position_, input.world_position_, albedo);
#endif
	
    float roughness = 0.7; // ラフ度 0.0:つるつる ～ 1.0:ざらざら (別名:glossiness, shininess)
    float metallic = 0.1; // 金属度 0.0:非金属   ～ 1.0:金属     (別名:metalness)

	//用意してはいるが、DxLbibが勝手にサンプラーを差し替えてくるため、
	//仕方なくDiffuseサンプラーを流用する。
    roughness = RoughnessTexture.Sample(DiffuseSampler, uv).r;
    metallic = MetallicTexture.Sample(DiffuseSampler, uv).r;

    float ao = 1.0f;
	
    float3 emissive = EmissionTexture.Sample(DiffuseSampler, uv).rgb * 200;
    emissive += DxLib_Common.Material.Ambient_Emissive.rgb * 2000;
	
	
	//----------------------------------------------------------
	// 出力
	//----------------------------------------------------------
    PS_OUTPUT_MRT output = PackSurfaceInfo(albedo, ao, N, roughness, metallic, input.world_position_.xyz, emissive);

	// 出力パラメータを返す
    return output;
}
