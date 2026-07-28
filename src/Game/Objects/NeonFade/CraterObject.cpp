#include "CraterObject.h"

namespace NeonFade
{
	int CraterObject::Init()
	{
		auto crater_mdl = AddComponent<ModelRenderer>();
		crater_mdl->SetModel("crater");

		return Super::Init();
	}
	void CraterObject::Update()
	{
		//lifetimeの時間管理や寿命による削除は親クラスがやっている
		Super::Update();

		//寿命の残り時間が少なくなると地面に沈み込むようにする
		if (lifetimer <= SINK_THRESHOLD)
		{
			// 残り時間が少なくなると沈む
			// ランダムな振動を加えて沈む
			transform->local_position.y -= Time::DeltaTime() * (SINK_SPEED + Random::Float01() * SINK_SPEED_VIBRATION);

			//平面方向にも少しだけランダムに振動させる
			transform->local_position += Random::Position(-SINK_VIBRATION_AMPLITUDE, SINK_VIBRATION_AMPLITUDE);
		}
	}
}