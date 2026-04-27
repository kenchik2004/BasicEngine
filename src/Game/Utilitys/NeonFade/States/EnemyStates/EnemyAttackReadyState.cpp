//---------------------------------------------------------------------------
//! @file   EnemyAttackReadyState.cpp
//! @brief  EnemyAttackReadyStateの実装。敵の攻撃準備状態の処理を行う
//---------------------------------------------------------------------------
#include "EnemyAttackReadyState.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade {
	EnemyAttackReadyState::EnemyAttackReadyState(Enemy* enemy_)
		:IState(static_cast<GameObject*>(enemy_))
	{
		owner_enemy = enemy_;
		animator = enemy_->animator.lock().get();
		rb = enemy_->rb.lock().get();
		target = SceneManager::Object::Get<Player>();
		std::function<bool()> request = [this]() {
			return next;
			};
		RegisterChangeRequest("attack_charge", request, 1);
	}
	void EnemyAttackReadyState::OnEnter(IStateMachine* machine)
	{
		next = false;
		ready_timer = max_ready_time + Random::Float01();
		animator->anim_speed = 2.0f;
		random_attack_distance = 3.0f + Random::Float01() * 5.0f;
		animator->Play("enemy_walk", true, Random::Float01());
	}
	void EnemyAttackReadyState::Update(IStateMachine* machine, float dt)
	{
		ready_timer -= dt;
		Vector3 mov_vec;

		auto enem_machine = static_cast<EnemyStateMachine*>(machine);
		mov_vec = target->transform->position - owner_enemy->transform->position;
		mov_vec = ProjectOnPlane(mov_vec, { 0,1,0 });

		if (mov_vec.magnitudeSquared() >= random_attack_distance * random_attack_distance || ready_timer > 0.0f) {

			mov_vec.normalize();
			owner_enemy->transform->SetAxisZ(mov_vec);
			mov_vec *= 10;
			mov_vec.y = rb->velocity.y;
			rb->velocity = mov_vec;
			return;
		}
		next = true;
	}
	void EnemyAttackReadyState::OnExit(IStateMachine* machine)
	{
		animator->anim_speed = 1.0f;
	}
}