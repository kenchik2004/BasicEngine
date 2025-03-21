#include "precompile.h"
#include "SampleCutIn.h"
#include "System/Components/ModelRenderer.h"


int SampleCutIn::Init()
{
	model = owner->GetComponent<ModelRenderer>();
	handle = MakeScreen(1920, 1080, true);
	model.lock()->anim_speed = 0.7f;
	SetTransColor(0, 0, 0);
	image = LoadGraph("data/cutin.mp4");
	SetCreateSoundTimeStretchRate(0.9f);
	se = LoadSoundMem("data/se.mp3");
	PlayMovieToGraph(image, DX_MOVIEPLAYTYPE_BCANCEL);
	PlaySoundMem(se, DX_PLAYTYPE_BACK);
	pos.x = 1920 * 2;
	pos.y = 1080 * 0.5f;
	Time::SetTimeScale(0.1f);
	SetTransColor(255, 0, 255);
	return 0;
}

void SampleCutIn::PostUpdate()
{
	timer -= Time::UnscaledDeltaTime();
	if (timer < 0)
		RemoveThisComponent();
	if (pos.x > 1920 * 0.5f)
		pos.x -= 1920 * 0.5 * Time::UnscaledDeltaTime() * 10;
	SetDrawScreen(handle);
	ClearDrawScreen();
	SetCameraPositionAndTargetAndUpVec(cast(owner->transform->position + cam_pos), cast(owner->transform->position + cam_lookat), cast(owner->transform->AxisY()));
	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));
	DrawRotaGraph3(pos.x, pos.y, 1920 * 0.25f, 1080 * 0.25f, 3, timer > 1.0f ? 1.0f / timer : 1, DEG2RAD(5), image, true);
	model.lock()->Draw();
	SetDrawScreen(DX_SCREEN_BACK);
	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));

}

void SampleCutIn::LateDraw()
{
	DrawRotaGraph(1920 * 0.5f, 1080 * 0.5f, 1.0f, 0, handle, true);
	DrawFormatString(0, 50, WHITE, "%.1f", timer);
}

void SampleCutIn::Exit()
{
	Time::SetTimeScale(1.0f);
	if(model.lock())
	model.lock()->anim_speed = 1.0f;
	DeleteSoundMem(se);
	DeleteGraph(handle);
	DeleteGraph(image);
}
