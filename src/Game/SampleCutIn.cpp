#include "precompile.h"
#include "SampleCutIn.h"
#include "System/Components/ModelRenderer.h"


int SampleCutIn::Init()
{
	model = owner.lock()->GetComponent<ModelRenderer>();
	handle = MakeScreen(SCREEN_W, SCREEN_H, true);
	model.lock()->anim_speed = 0.7f;
	SetTransColor(0, 0, 0);
	image = LoadGraph("data/cutin.mp4");
	SetCreateSoundTimeStretchRate(0.9f);
	se = LoadSoundMem("data/se.mp3");
	PlayMovieToGraph(image, DX_MOVIEPLAYTYPE_BCANCEL);
	PlaySoundMem(se, DX_PLAYTYPE_BACK);
	pos.x = SCREEN_W * 2;
	pos.y = SCREEN_H * 0.5f;
	Time::SetTimeScale(0.1f);
	SetTransColor(255, 0, 255);
	return 0;
}

void SampleCutIn::PostUpdate()
{
	timer -= Time::UnscaledDeltaTime();
	if (timer < 0)
		RemoveThisComponent();
	if (pos.x > SCREEN_W * 0.5f)
		pos.x -= SCREEN_W * 0.5 * Time::UnscaledDeltaTime() * 10;
	SetDrawScreen(handle);
	ClearDrawScreen();
	SetCameraPositionAndTargetAndUpVec(cast(owner.lock()->transform->position + cam_pos), cast(owner.lock()->transform->position + cam_lookat), cast(owner.lock()->transform->AxisY()));
	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));
	DrawRotaGraph3(pos.x, pos.y, SCREEN_W * 0.25f, SCREEN_H * 0.25f, 2, 0.5f, DEG2RAD(5), image, true);
	model.lock()->Draw();
	SetDrawScreen(DX_SCREEN_BACK);
	SetCameraNearFar(0.1f, 3000.0f);
	SetupCamera_Perspective(TO_RADIAN(45.0f));

}

void SampleCutIn::LateDraw()
{
	DrawRotaGraph(SCREEN_W * 0.5f, SCREEN_H * 0.5f, 1, 0, handle, true);
	DrawFormatString(0, 50, WHITE, "%.1f", timer);
}

void SampleCutIn::Exit()
{
	Time::SetTimeScale(1.0f);
	if (model.lock())
		model.lock()->anim_speed = 1.0f;
	DeleteSoundMem(se);
	DeleteGraph(handle);
	DeleteGraph(image);
}
