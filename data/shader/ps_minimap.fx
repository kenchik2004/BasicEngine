#include "dxlib_ps.h.fx"


cbuffer CB_PS_MINIMAP : register(b12)
{
    float MINIMAP_RADIUS; //!< ミニマップ半径
    float MINIMAP_BORDER;
    float2 MINIMAP_SCREEN_POS;
}

PS_OUTPUT main(PS_INPUT input)
{
    float2 position = input.position_.xy;
    float2 center = MINIMAP_SCREEN_POS;
    float2 offset = position - center;
    float distance = length(offset);
    // ミニマップの外側は透明にする
    if (distance > MINIMAP_RADIUS)
    {
        discard; // ピクセルを破棄して透明にする
    }
    // ミニマップの色を設定（ここでは単純に白色に設定）
    PS_OUTPUT output;
    output.color0_ = DiffuseTexture.Sample(DiffuseSampler, input.uv0_); // 白色
    output.color0_ *= input.diffuse_; // ディフューズカラーを掛ける
    output.color0_.a = 1.0; // アルファ値を1.0に設定
    if (distance > MINIMAP_RADIUS - MINIMAP_BORDER)
    {
        float step = distance - (MINIMAP_RADIUS - MINIMAP_BORDER);
        step *= 1.0 / MINIMAP_BORDER;

        output.color0_.a = lerp(1.0, 0.0, step);

    }
    return output;
}