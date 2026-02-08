#include "Game/Utilitys/NeonFade/States/EnemyStates/EnemyLeaderAttackState.h"
#include "Game/Objects/NeonFade/Enemy.h"

namespace NeonFade {
	EnemyLeaderAttackState::EnemyLeaderAttackState(Enemy* owner_)
		:IState(static_cast<GameObject*>(owner_))
	{
		std::function<bool()> attack_to_idle = [state = this]() {
			return state->attack_timer >= state->ATTACK_TIME;
			};
		RegisterChangeRequest("idle", attack_to_idle, 1);
		enemy = owner_;
		rb = enemy->rb.lock().get();
		animator = enemy->animator.lock().get();
	}
	void EnemyLeaderAttackState::OnEnter(IStateMachine* machine)
	{
		attack_timer = 0.0f;
		animator->Play("enemy_instruct", true, 0.0f, 0.2f, false);
	}
	void EnemyLeaderAttackState::OnExit(IStateMachine* machine)
	{
	}
	void EnemyLeaderAttackState::Update(IStateMachine* machine, float dt)
	{
		attack_timer += dt;
		if (attack_timer >= ATTACK_TIME) {
			//攻撃終了
			attack_timer = 0.0f;
		}
		//0.5秒ごとにジャンプして扇動してる感を出す
		if (fmodf(attack_timer, 0.5f) <= dt) {
			rb->AddForce(Vector3(0, 10, 0),ForceMode::VelocityCange);
		}
	}
}