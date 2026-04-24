#include "EnemyWelterState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade
{
	EnemyWelterState::EnemyWelterState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		enemy = owner_;
		rb = enemy->rb.lock().get();
		animator = enemy->animator.lock().get();
		std::function default_change = [this]() {
			return timer >= WELTER_TIME;
			};
		RegisterChangeRequest("idle", default_change, 1);

		if (!welter_se)
			welter_se = AudioManager::CloneByName(u8"welter_se");
	}
	void EnemyWelterState::OnEnter(IStateMachine* machine)
	{
		timer = 0.0f;
		enemy->animator.lock()->PlayIfNoSame("enemy_escape", true, 0.0f, 0.1f, false);
		welter_se->PlayOneShot();
	}
	void EnemyWelterState::OnExit(IStateMachine* machine)
	{
	}
	void EnemyWelterState::Update(IStateMachine* machine, float dt)
	{
		timer += dt;
		Transform* trns = enemy->transform.get();
		trns->AddRotation(Vector3(0, 720.0f * dt * Random::Range(-1.0f, 1.0f), 0));

		Vector3 forward = enemy->transform->AxisZ();
		rb->velocity = forward * 15.0f;

	}
}