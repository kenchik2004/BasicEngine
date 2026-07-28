#include "precompile.h"
#include "EffectDrawerObject.h"
#include "../Effekseer/EffekseerForDXLib.h"

void EffectDrawerObject::LateDraw()
{
	auto current_rt = GetRenderTarget();
	DrawEffekseer3D_End();
	GetEffekseer3DRenderer()->ResetRenderState();
	SetRenderTarget(current_rt);

}
