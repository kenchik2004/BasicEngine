#include "dxlib_ps.h.fx"


cbuffer SampleCB : register(b5) {
	float sampleparam1;
	float sampleparam2;
	float sampleparam3;
};

// 0.0～1.0の範囲でランダムな値を生成
float InterleavedGradientNoise(float2 position_screen)
{
	float3 magic = float3(0.06711056f, 0.00583715f, 52.9829189f);
	return frac(magic.z * frac(dot(position_screen, magic.xy)));
}

float BlockNoise(float2 blocks)
{
	float2 p = trunc(blocks);
	return InterleavedGradientNoise(p);
}


//---------------------------------------------------------------------------
// Vogel disk sampling
//---------------------------------------------------------------------------
float2 VogelDiskSample(int index, int numSamples, float angleBias = 0.0f)
{
	// Vogel disk sampling
	// https://www.shadertoy.com/view/4l3yRM

	// 
	const float GOLDEN_ANGLE = 2.399963229728653; // 黄金角 (黄金比に基づく角度)

	float theta = GOLDEN_ANGLE * float(index) + angleBias;
#if 0
	// リファレンス実装
	float r = sqrt(float(index) + 0.5) / sqrt(float(numSamples));
	return float2(r * cos(theta), r * sin(theta));
#else
	// 最適化版
	float r = sqrt((float(index) + 0.5) / float(numSamples));
	return sin(float2(theta + (PI / 2.0), theta)) * r;
#endif
}



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


PS_OUTPUT FLAME_SHADER(PS_INPUT psin)
{
	PS_OUTPUT psout;

	//黒体放射の近似
	//エフェクトの適用率をディスプレイスメントマップの赤チャンネルから取得
	float4 effectColor = NormalTexture.Sample(NormalSampler, psin.uv0_);
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
	psout.color0_.rgba = float4(blackbody_color, blackbody_alpha);




	return psout;
}
PS_OUTPUT GUWAGUWA_SHADER(PS_INPUT psin)
{
	PS_OUTPUT psout;

	// テクスチャカラーの読み込み
	float2 UV = psin.uv0_;

	UV = UV * 2 - 1; // 0～1 → -1～1
	//UVを中心位置から放射状に歪ませる
	float2 guwaguwa_vec = (frac(sampleparam1) * sampleparam3 - length(UV));
	float guwaguwa_length = length((guwaguwa_vec));


	//valueよりも外側にある場合、無効化(-1)
	float active_length = length(guwaguwa_vec) > 0 ? guwaguwa_length : -1;
	//さらに、valueより0.6以上内側(正数しかないのでlengthで判定)にある場合も無効化(-1)
	active_length = active_length > 0.6 ? -1 : active_length;


	UV = lerp(UV, normalize(UV) * -guwaguwa_length, active_length > 0 ? active_length * 0.5 : 0);

	//参考サイトではドット絵っぽくなっているので、ちょっと再現してみる
 // -シンプルにBlockNoiseを使う
 //------------------------------------------------

	float2 offset = BlockNoise(psin.uv0_ * 128) * active_length * 0.5;
	UV = UV + (active_length > 0 ? offset : float2(0, 0));
	//------------------------------------------------   


	//上の条件を満たす場合にのみ、UVを中心から内側に向かって伸ばす

	// -1~1 → 0～1
	UV = (UV + 1) * 0.5;

	float4 texc;



	texc = DiffuseTexture.Sample(DiffuseSampler, UV);


	psout.color0_ = texc;
	return psout;


}



PS_OUTPUT main(PS_INPUT psin)
{
	PS_OUTPUT psout = FLAME_SHADER(psin);
	if (sampleparam2 > 1)
	{
		psout = GUWAGUWA_SHADER(psin);
	}
	//psout.color0_ = float4(0, 0, 0, 0);
	return psout;


}
