#include "BasicEnemyBrain.h"
#include "Game/Utilitys/NeonFade/StateMachines/EnemyStateMachine.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
namespace NeonFade {
	BasicEnemyBrain::BasicEnemyBrain(EnemyStateMachine* state_machine_, PlayerWP player_)
		:AbstractEnemyBrain(state_machine_, player_)
	{

	}
	void BasicEnemyBrain::Think()
	{
		const float dt = Time::UnscaledDeltaTime();
		if (i_frame_timer > 0.0f)
			i_frame_timer -= dt;

		bool knock_back_bool = knock_back;
		state_machine->is_damaged = is_damaged;
		state_machine->knock_back = knock_back_bool;

		if (player)
		{
			if (auto player_sp = player.lock())
			{
				Vector3 to_player = player_sp->transform->position - state_machine->enemy->transform->position;
				to_player.y = 0;
				to_player = to_player.getNormalized() * 5;
				if (!knock_back_bool)
					input_vec = to_player;
			}

		}
		if (state_machine->enemy->transform->position.y < -50.0f)
		{
			Die();
		}

		state_machine->move_vec = input_vec;
		state_machine->is_attacking = false;
		state_machine->is_dead = hp == 0;
		knock_back = false;
		is_damaged = false;
	}
	void BasicEnemyBrain::Damage(u32 damage, bool ignore_i_frame) {
		if (!ignore_i_frame && i_frame_timer > 0.0f)
			return;
		if (damage > hp)
		{
			Die();
			return;
		}
		hp -= damage;
		i_frame_timer = I_FRAME;
		is_damaged = true;
	}
	void BasicEnemyBrain::Die()
	{
		hp = 0;
	}
	void BasicEnemyBrain::KnockBack(Vector3 knock_back_vec)
	{
		input_vec = knock_back_vec;
		knock_back = true;
		state_machine->move_vec = input_vec;
	}
	void BasicEnemyBrain::DebugDraw()
	{

	}
}