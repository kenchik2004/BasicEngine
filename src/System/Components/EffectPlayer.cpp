#include "EffectPlayer.h"
#include "../Effekseer/EffekseerForDXLib.h"

EffectPlayer::EffectPlayer(std::string_view name)
	:Component()
{
	Load(name);
}

int EffectPlayer::Init()
{
	return 0;
}


void EffectPlayer::LateDraw()
{
	if (!is_playing)
		Play();
	is_playing = !IsEffekseer3DEffectPlaying(playing_handle);
	if (!is_playing)
	{
		playing_handle = -1;
		return;
	}
	Vector3& pos = owner.lock()->transform->position;
	Quaternion& rot = owner.lock()->transform->rotation;
	Vector3& scale = owner.lock()->transform->scale;
	Vector3 unit_axis = { 0,1,0 };
	float radian = 0;
	rot.toRadiansAndUnitAxis(radian, unit_axis);
	Effekseer::Vector3D basis = { unit_axis.x,unit_axis.y,unit_axis.z };
	SetPosPlayingEffekseer3DEffect(playing_handle, pos.x, pos.y, pos.z);
	GetEffekseer3DManager().Get()->SetRotation(playing_handle, basis, radian);
	SetScalePlayingEffekseer3DEffect(playing_handle, scale.x, scale.y, scale.z);
	SetSpeedPlayingEffekseer3DEffect(playing_handle, speed);
	DrawEffekseer3D_Draw(playing_handle);
}

void EffectPlayer::Play()
{
	if (effect_handle < 0)
		return;
	if (is_playing)
		Stop();

	playing_handle = PlayEffekseer3DEffect(effect_handle);

	is_playing = true;
}
void EffectPlayer::Stop()
{
	StopEffekseer3DEffect(playing_handle);
	playing_handle = -1;
	is_playing = false;
}

void EffectPlayer::Load(std::string_view effect_name_)
{
	effect_name = effect_name_;
	effect_handle = LoadEffekseerEffect(effect_name.c_str());
}

void EffectPlayer::Exit()
{
	if (effect_handle >= 0) {
		DeleteEffekseerEffect(effect_handle);
		effect_handle = -1;
	}
}
