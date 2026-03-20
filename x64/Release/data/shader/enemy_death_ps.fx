#include "dxlib_ps.h.fx"
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

struct PS_OUTPUT_MRT
{
	float4 color0_ : SV_Target0;
	float4 color1_ : SV_Target1;
	float4 color2_ : SV_Target2;
};


cbuffer SampleCB : register(b5) {
	float sampleparam1;
	float sampleparam2;
	float sampleparam3;
};



//---------------------------------------------------------------------------
// 黒体放射の色近似関数（最適化版）
//---------------------------------------------------------------------------
// heat: 0.0～1.0 (0.0=低温の赤, 1.0=黄橙色)
// 1000K～3000K程度の炎の色域を近似
float3 BlackBodyRadiation(float heat)
{
	float temperature = clamp(heat, 0, 2);
	// 赤チャンネル: 常に高い値を維持（炎は赤が強い）
	//float r = temperature < 1.0 ? saturate(temperature * 2) : 1 - saturate(temperature * 0.5);
	float r = temperature < 1.2 ? temperature * 2 : 1 - temperature * 0.5;

	// 緑チャンネル: heatに応じて上昇（赤→オレンジ→黄色）
	// 2次曲線で滑らかに変化
	//float g = heat * heat;
	float g = temperature < 1.5 ? temperature * temperature : 1 - temperature * temperature * 0.5;

	// 青チャンネル: 低温では0、高温でわずかに上昇
	// 3次曲線でゆっくり立ち上がる
	//float b = heat * heat * heat * 0.3;
	float b = temperature > 1.2 ? temperature * temperature * 0.45 : temperature * temperature * temperature * 0.3;

	return saturate(float3(r, g, b));
}


float4 FLAME_SHADER(PS_INPUT_MODEL psin)
{
	float4 psout;

	//黒体放射の近似
	//エフェクトの適用率をディスプレイスメントマップの赤チャンネルから取得
	float4 effectColor = SpecularTexture.Sample(SpecularSampler, psin.uv0_);
	float heat_value = saturate(length(effectColor.r));

	// 時間経過に応じて変化する熱の範囲を計算
	float heat_range = frac(sampleparam1 * 0.3 * sampleparam3);
	float heat_distance = (heat_value - heat_range);

	// 熱の距離に基づいて強度を調整
	//0.15以上離れている部分はまだ燃えていないので、例外値として2.0を設定
	heat_distance = heat_distance > 0.15 ? 2.0 : saturate(heat_distance / 0.15);



	// 熱の強さに応じて黒体放射色を適用
		// 黒体放射の色を計算
	float3 blackbody_color = BlackBodyRadiation(saturate(heat_distance));

	// テクスチャカラーの読み込み
	float4 texc = DiffuseTexture.Sample(DiffuseSampler, psin.uv0_);

	//先ほど設定した例外値を使って、熱が及ばない部分はテクスチャカラーをそのまま使う
	blackbody_color = heat_distance > 1.0 ? texc.rgb : blackbody_color;

	//アルファ値も同様に調整
	//熱が通り過ぎた(0.01以下)部分は透明にする
	//それ以外(熱がある部分)はテクスチャのアルファを使う
	float blackbody_alpha = heat_distance < 0.01 ? 0.0 : texc.a;
	psout.rgba = float4(blackbody_color, blackbody_alpha);


	return psout;
}



//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_MRT main(PS_INPUT_MODEL input)
{
	float2 uv = input.uv0_;

	float3 N = normalize(input.normal_); // 法線

	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
	N = Normalmap(N, input.world_position_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
	float4 textureColor = FLAME_SHADER(input);
	textureColor = saturate(textureColor);
	textureColor.rgb = pow(textureColor.rgb, 2.2);

	// アルファテスト
	if (textureColor.a < 0.5)
		discard;

	float3 albedo = textureColor.rgb * input.diffuse_.rgb;


#if 1	
	float roughness = 0.5; // ラフ度 0.0:つるつる ～ 1.0:ざらざら (別名:glossiness, shininess)
	float metallic = 1.0; // 金属度 0.0:非金属   ～ 1.0:金属     (別名:metalness)

	roughness = RoughnessTexture.Sample(RoughnessSampler, uv).r;
	metallic = MetallicTexture.Sample(MetallicSampler, uv).r;
#else
	//スペキュラマップがあるもでるはこっち
	float3 specularColor = SpecularTexture.Sample(SpecularSampler, uv).rgb;

	// 疑似的に "metallic" を推定する（明るい＝金属っぽい）
	float metallic = saturate(dot(specularColor, float3(0.333, 0.333, 0.333)) * 2.0);

	// 疑似的に "roughness" を推定（明るいほどツルツル）
	float roughness = 1.0 - saturate(dot(specularColor, float3(0.333, 0.333, 0.333)));
#endif
	float ao = 1.0f;

	//----------------------------------------------------------
	// 出力
	//----------------------------------------------------------
	PS_OUTPUT_MRT output;
	float3 emissive = EmissionTexture.Sample(EmissionSampler, uv).rgb;
	output.color0_ = float4(albedo, ao);
	output.color1_ = float4(NormalEncode(N).xy, roughness, metallic);
	output.color2_ = float4(input.world_position_.xyz, 1);
	// 出力パラメータを返す
	return output;
}
