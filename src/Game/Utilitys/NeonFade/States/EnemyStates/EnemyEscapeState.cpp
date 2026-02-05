#include "EnemyEscapeState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade {
	EnemyEscapeState::EnemyEscapeState(Enemy* owner_, SafeWeakPtr<Object> leder_) :
		IState(static_cast<GameObject*>(owner_))
	{
		enemy = owner_;
		escape_timer = ESCAPE_TIME;
		rb = enemy->rb.lock().get();
		animator = enemy->animator.lock().get();
		player = SceneManager::Object::Get<Player>().get();
		leader = leder_;
		std::function escape_end = [this]() {
			return exit_flag;
			};
		RegisterChangeRequest("idle", escape_end, 0);

	}

	void EnemyEscapeState::OnEnter(IStateMachine* machine)
	{
		escape_timer = ESCAPE_TIME;
		animator->Play("enemy_escape", true, Random::Float01(), 0.2f, false);
		exit_flag = false;
	}

	void EnemyEscapeState::OnExit(IStateMachine* machine)
	{
	}

	void EnemyEscapeState::Update(IStateMachine* machine, float dt)
	{
		escape_timer -= dt;
		Vector3 mov = enemy->transform->AxisZ();
		if (leader)
		{
			Vector3 to_leader = leader->transform->position - enemy->transform->position;
			mov = Lerp(mov, to_leader.getNormalized(), 0.2f);
			if (to_leader.magnitude() < 5.0f)
				mov = Lerp(mov, -to_leader.getNormalized(), 0.3f);
		}
		else {

			Vector3 to_player = player->transform->position - enemy->transform->position;
			mov = Lerp(mov, -to_player.getNormalized(), 0.1f);

		}
		mov.normalize();
		if (!mov.isZero())
			enemy->transform->SetAxisZ(mov);
		mov *= 10;
		mov.y = rb->velocity.y;
		rb->velocity = mov;
		if (escape_timer <= 0.0f)
		{
			exit_flag = true;	// タイムアップで逃走終了
		}

	}

}