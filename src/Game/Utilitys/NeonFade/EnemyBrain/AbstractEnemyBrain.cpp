#include "AbstractEnemyBrain.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

namespace NeonFade {

	AbstractEnemyBrain::AbstractEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		: state_machine(state_machine_), player(player_) {

		if (state_machine_)
			body = state_machine_->enemy;
		if (instance_count == 0)
			hp_bar = TextureManager::Create("enem_hp_bar", 300, 50, DXGI_FORMAT_R8G8B8A8_UNORM);
		instance_count++;
	}
	AbstractEnemyBrain::~AbstractEnemyBrain()
	{
		instance_count--;
		if (instance_count == 0)
			hp_bar.reset();
	}
	void AbstractEnemyBrain::DrawHPDebug()
	{
		Vector3 pos = cast(ConvWorldPosToScreenPos(cast(body->transform->position + Vector3(0, 15, 0))));
		if (pos.z < 0)
			return;

		auto cur_rt = GetRenderTarget();

		SetRenderTarget(hp_bar.get());
		DrawBox(0, 0, 300, 50, Color::GRAY, true);
		DrawBoxAA(0, 0, 3 * hp, 50, Color::GREEN, true);

		SetRenderTarget(cur_rt);

		DrawGraphF(pos.x, pos.y, *hp_bar, true);
	}
}

