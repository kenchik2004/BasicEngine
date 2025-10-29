//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "lighting.h.fx"
#include "camera.h.fx"
#include "shadow.h.fx"

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

//==============================================================
// IBL
//==============================================================
TextureCube IBL_Diffuse : register(t14);
TextureCube IBL_Specular : register(t15);


//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;

    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_); // 法線
    float3 V = normalize(eye_position_ - input.world_position_);
	
	
    float3 light = light_info_[0].direction_; // ライトのポジション
	
    float3 L = normalize(-light);
	
	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv) * input.diffuse_;
    textureColor = saturate(textureColor);
	
    // アルファテスト
    if (textureColor.a < 0.5)
        discard;

	// リニア化 sRGBのテクスチャ → Linearのテクスチャ
    textureColor.rgb = pow(textureColor.rgb, 2.2);
	
    float3 albedo = textureColor.rgb;
	
    output.color0_ = textureColor;
    //output.color0_.rgb = N; // * input.diffuse_;
    //output.color0_.a = 1;
    //return output;
	
	
	
	
	
    float distance = input.position_.w;
	
	
	// LERP: 線形補間 Linear intERPoration
	// result = lerp(A, B, ratio);			0 <= ratio <= 1
	// ※ratioは範囲外になっても動作するが、期待した結果にならない場合はsaturateする
	
    // saturate 飽和 (入力値を0.0	-1.0の間に収める)
	// float n;
	// if (n < 0.0) n = 0.0;
	// if (n > 1.0) n = 1.0;
	// ※saturateは0サイクルで実行可能。コストゼロ

	// abs 絶対値 absolute
	// if(n < 0.0) n = -n;
	// ※absは0サイクルで実行可能。コストゼロ
	// -n;		// 1サイクル
    // abs(n)	// 0サイクル

	// max 最大値
	// min 最小値
	// result = max(a, b);
	// result = min(a, b);
	// aとbの大きい方(または小さい方)を選択する	
	
	// clamp 範囲制限
	// result clamp(x, 0.0, 2.0);
	// 最小値～最大値に収める
	
	
	
	//-------------------------------------------------------------
	// フォグ表現
	//-------------------------------------------------------------
    float ratio = saturate(distance * 0.005);
   //output.color0_.rgb = lerp(output.color0_.rgb, float3(0.4, 0.6, 1), ratio);
	
    int2 position = int2(input.position_.xy); // 画面上のピクセル位置
	

    
    float3 H = normalize(L + V); // ハーフベクトル
	
	
	

    float3 lightColor = light_info_[0].light_color_; // 光源の明るさ, 色
    float roughness = 0.5; // ラフ度 0.0:つるつる ～ 1.0:ざらざら (別名:glossiness, shininess)
    float metallic = 0.5; // 金属度 0.0:非金属   ～ 1.0:金属     (別名:metalness)
	
    metallic = SpecularMapTexture.Sample(SpecularMapSampler, uv).r;
 
	
	
    float3 diffuse;
    float3 specular;
	
    lighting(lightColor,
			 N, L, V, H,
			 roughness, metallic,
			 albedo,
			 diffuse, specular);
	

	
    float shadow = getShadow(input.position_, input.world_position_);
	
    shadow = shadow * 0.5 + 0.5;
	
	
	

	
#if 0
	
	//==========================================================
	// Image Based Lighting
	//==========================================================
	{
        float3 ibl_diffuse = IBL_Diffuse.SampleLevel(DiffuseSampler, N, 0).rgb * Kd;
        ibl_diffuse *= textureColor.rgb;
		
        output.color0_.rgb = ibl_diffuse;
		
		// 反射ベクトルR
        float3 R = reflect(-V, N);
		
        float mip = roughness * 7;
        float3 ibl_specular = IBL_Specular.SampleLevel(DiffuseSampler, R, mip).rgb;
        float3 environmentBrdf = pow(1.0 - max(roughness, NdotV), 3.0) + specularColor;
        ibl_specular *= environmentBrdf;

        output.color0_.rgb = ibl_diffuse * (1.0 - metallic) + ibl_specular;

        output.color0_.rgb *= 2;
    }
#else
	
	// ambient light 環境光
	// 周辺の明るさを近似
    float3 ambient = float3(1.0, 1.0, 1.0);
	
    output.color0_.rgb *= diffuse + ambient;
    output.color0_.rgb += specular;
    
	
#endif

    output.color0_.rgb *= shadow;

	
	
	//----------------------------------------------------------
	// ガンマ補正 Linear → sRGB
	// ※表示用のガンマカーブ
	//----------------------------------------------------------
	
	// pow べき乗
	// pow(n, x);	nのx乗
    output.color0_.rgb = pow(output.color0_.rgb, 1.0 / 2.2);
	
	
	// 出力パラメータを返す
    return output;
}
