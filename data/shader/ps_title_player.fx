//----------------------------------------------------------------------------
//!	@file	ps_model_gbuffer.fx
//!	@brief	MV1モデルピクセルシェーダー / GBuffer出力
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "shadow.h.fx"
#include "gbuffer.h.fx"
//#define DEBUG_CASCADE

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

cbuffer TitlePlayerInfo : register(b12)
{
    float3 spawn_start_pos;
    float character_height;
    float spawn_timer;

}
float CalcFadeLevel(float3 world_pos)
{
    float fade_start = spawn_start_pos.y;
    float border_pos = fade_start + character_height * (spawn_timer > 2.0 ? 1000.0 : spawn_timer);
    float fade_level = border_pos - world_pos.y;
    return fade_level;
    
}
float CalcEmissionLevel(float fade_level)
{
    static const float border_size = 0.5; // フェードの境界の幅
    static const float max_fade_level = 1.0; // フェードレベルの最大値
    float emission = smoothstep(0.0, border_size, fade_level);
    return fade_level > max_fade_level ? 0.000 : emission; // fade_levelが0以上border_size以下の範囲で0から1に変化するようにする
}

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_MRT main(PS_INPUT_MODEL input)
{
    
    float fade_level = CalcFadeLevel(input.world_position_);
    if (fade_level < 0.0)
        discard;
    float2 uv = input.uv0_;

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
    textureColor.rgb = pow(textureColor.rgb, 2.2);
    
    
    if (input.world_position_.y > spawn_start_pos.y + character_height * spawn_timer)
    {
        textureColor.a = 0.0;
    }

	// アルファテスト
    if (textureColor.a < 0.5)
        discard;
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
	
    static const float3 base_emissive_color = float3(0.0, 10000.0, 10000.0); // ベースの発光色
    float3 emissive = EmissionTexture.Sample(DiffuseSampler, uv).rgb * 200;
    emissive += DxLib_Common.Material.Ambient_Emissive.rgb * 2000;
    emissive = lerp(emissive, base_emissive_color, CalcEmissionLevel(fade_level)); // フェードレベルに応じて発光色を変化させる
	
	//----------------------------------------------------------
	// 出力
	//----------------------------------------------------------
    PS_OUTPUT_MRT output = PackSurfaceInfo(albedo, ao, N, roughness, metallic, input.world_position_.xyz, emissive);

	// 出力パラメータを返す
    return output;
}
