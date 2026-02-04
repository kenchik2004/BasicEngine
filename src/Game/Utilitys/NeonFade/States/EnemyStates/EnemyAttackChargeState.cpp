#include "EnemyAttackChargeState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	EnemyAttackChargeState::EnemyAttackChargeState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		owner_enemy = owner_;
		animator = owner_enemy->animator.lock().get();
		rb = owner_enemy->rb.lock().get();
		std::function<bool()> charge_to_attack = [this]() {
			return charge_timer >= CHARGE_TIME;
			};
		RegisterChangeRequest("attack_main", charge_to_attack, 0);
	}
	void EnemyAttackChargeState::OnEnter(IStateMachine* machine)
	{
		animator->Play("enemy_attack_charge");
		charge_timer = 0.0f;
		rb->velocity = { 0,0,0 };
	}
	void EnemyAttackChargeState::OnExit(IStateMachine* machine)
	{
	}
	void EnemyAttackChargeState::Update(IStateMachine* machine, float dt)
	{
		charge_timer += dt;
		if (charge_timer < 0.6f) {
			auto enem_machine = static_cast<EnemyStateMachine*>(machine);
			Vector3 mov = enem_machine->move_vec;
			mov = ProjectOnPlane(mov, { 0,1,0 });
			if (mov.isFinite() && !mov.isZero())
				owner_enemy->transform->SetAxisZ(mov);
			if (mov.magnitudeSquared() >= 3.0f * 3.0f)
				rb->velocity = mov.getNormalized() * 3.0f;
			else
				rb->velocity = { 0,rb->velocity.y,0 };
		}
	}
}