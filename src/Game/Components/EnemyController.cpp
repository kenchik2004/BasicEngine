#include "precompile.h"
#include "EnemyController.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/Player.h"

#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"

namespace NeonFade {
	int EnemyController::Init()
	{
		SafeSharedPtr<Enemy> owner_enemy = SafeStaticCast<Enemy>(owner.lock());
		state_machine = make_safe_unique<EnemyStateMachine>(owner_enemy.get());
		player = SceneManager::Object::Get<NeonFade::Player>();
		return 0;
	}

	void EnemyController::Update()
	{



		invincible_timer -= Time::UnscaledDeltaTime();
		state_machine->knock_back = knock_back;

		if (knock_back)
		{
			owner->transform->SetAxisZ(knock_back_vec);
			knock_back = false;
		}

		state_machine->is_damaged = is_damaged;
		if (is_damaged)
		{
			is_damaged = false;
		}
		Vector3 player_dist = { 0,0,0 };
		if (auto player_lock = player.lock())
		{
			is_attacking = false;
			player_dist = player_lock->transform->position - owner.lock()->transform->position;
			player_dist.y = 0;
			if (player_dist.magnitudeSquared() <= 8.0f * 8.0f) {
				state_machine->move_vec = player_dist;
				is_attacking = true;
			}
		}
		state_machine->is_attacking = is_attacking;



		state_machine->Update(Time::UnscaledDeltaTime());
	}

	void EnemyController::Exit()
	{
	}

	void EnemyController::LateDebugDraw()
	{
		state_machine->DebugDraw();
	}

	void EnemyController::Damage(int damage)
	{
		if (invincible_timer <= 0.0f && !is_dead)
		{
			invincible_timer = INVINCIBLE_TIME;
			hp -= damage;
			is_damaged = true;
			if (hp > 0)
				state_machine->ChangeState("damage");
			else
				is_dead = true;
		}
		state_machine->is_dead = is_dead;
	}

	void EnemyController::Down(Vector3 vec, int damage)
	{
		if (invincible_timer <= 0.0f && !is_dead)
		{
			knock_back = true;
			knock_back_vec = vec;
			invincible_timer = INVINCIBLE_TIME;
			hp -= damage;
			is_damaged = true;
			state_machine->ChangeState("down");
			if (hp <= 0)
				is_dead = true;
		}
		state_machine->is_dead = is_dead;
	}

	void EnemyController::OnTriggerEnter(const HitInfo& hit_info)
	{
		state_machine->OnTriggerEnter(hit_info);
	}
}
