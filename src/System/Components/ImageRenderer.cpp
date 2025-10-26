#include "Precompile.h"
#include "System/Components/ImageRenderer.h"
#include "System/MaterialManager.h"
#include "System/Utils/Render.h"



int ImageRenderer::Init()
{
	ui_owner = SafeStaticCast<UIObject>(owner.lock());
	material = MaterialManager::GetDefaultMat2D();
	return Super::Init();
}

void ImageRenderer::Update()
{
}

void ImageRenderer::LateDraw()
{

	DrawMain();
}

void ImageRenderer::DrawMain()
{
	Vector3 draw_pos = ui_owner->GetDrawPos();
	Vector3 scale = ui_owner->transform->scale;
	VERTEX2DSHADER vert[4];
	u8 alpha_u8 = static_cast<u8>(std::clamp(255 * alpha, 0.0f, 255.0f));
	vert[0].pos.x = draw_pos.x;
	vert[0].pos.y = draw_pos.y;
	vert[0].pos.z = 0.0f;
	vert[0].rhw = 1.0f;
	vert[0].dif = { 255,255,255,alpha_u8 };
	vert[0].u = 0.0f;
	vert[0].v = 0.0f;
	vert[1].pos.x = draw_pos.x + scale.x;
	vert[1].pos.y = draw_pos.y;
	vert[1].pos.z = 0.0f;
	vert[1].rhw = 1.0f;
	vert[1].dif = { 255,255,255,alpha_u8 };
	vert[1].u = 1.0f;
	vert[1].v = 0.0f;
	vert[2].pos.x = draw_pos.x;
	vert[2].pos.y = draw_pos.y + scale.y;
	vert[2].pos.z = 0.0f;
	vert[2].rhw = 1.0f;
	vert[2].dif = { 255,255,255,alpha_u8 };
	vert[2].u = 0.0f;
	vert[2].v = 1.0f;
	vert[3].pos.x = draw_pos.x + scale.x;
	vert[3].pos.y = draw_pos.y + scale.y;
	vert[3].pos.z = 0.0f;
	vert[3].rhw = 1.0f;
	vert[3].dif = { 255,255,255,alpha_u8 };
	vert[3].u = 1.0f;
	vert[3].v = 1.0f;

	SetUsePixelShader(*material->GetPixelShader());
	for (u32 i = 0; i < 16; i++)
	{
		SetUseTextureToShader(i, -1);
	}
	for (u32 i = 0; i < static_cast<u32>(Material::TextureType::Max); i++) {
		auto texture = material->GetTexture(static_cast<Material::TextureType>(i));
		if (texture) {
			SetUseTextureToShader(i, *texture.get());
		}
		else {
			SetUseTextureToShader(i, -1);
		}
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	SetDrawMode(DX_DRAWMODE_BILINEAR);
	DrawPrimitive2DToShader(vert, 4, DX_PRIMTYPE_TRIANGLESTRIP);
	SetUsePixelShader(-1);
	for (u32 i = 0; i < static_cast<u32>(Material::TextureType::Max); i++) {
		SetUseTextureToShader(i, -1);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetDrawMode(DX_DRAWMODE_NEAREST);

#if 0
	alpha = std::clamp<int>(alpha, 0, 255);
	switch (draw_type) {
	case ImageRenderer::DEFAULT:

		draw_pos += scale * 0.5f;
		DrawRotaGraphF(draw_pos.x, draw_pos.y, 1.0f, 0.0f, *image, true);

		break;
	case ImageRenderer::CLAMP:
		DrawRectGraphF(draw_pos.x, draw_pos.y, 0, 0, scale.x, scale.y, *image, true);
		break;
	case ImageRenderer::EXTEND:
		DrawExtendGraphF(draw_pos.x, draw_pos.y, draw_pos.x + scale.x, draw_pos.y + scale.y, *image, true);
		break;
	default:
		break;
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetDrawMode(DX_DRAWMODE_NEAREST);
#endif
}

float2 ImageRenderer::GetImageSize()
{
	float2 ret;
	if (material)
		GetGraphSizeF(*material->GetTexture(Material::TextureType::Diffuse), (float*)&ret.x, (float*)&ret.y);
	return ret;
}

void ImageRenderer::Exit()
{
	material = nullptr;
	Super::Exit();
}
