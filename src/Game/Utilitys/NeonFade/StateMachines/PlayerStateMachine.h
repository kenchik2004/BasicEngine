#pragma once
#include "Game/Utilitys/NeonFade/StateMachines/IStateMachine.h"

namespace NeonFade
{
	class Player;
	class PlayerStateMachine :
		public IStateMachine
	{
	public:
		PlayerStateMachine(Player* owner_);
		virtual ~PlayerStateMachine();
		void DebugDraw() override;
		void Update(float dt) override;

	public:
		Player* player = nullptr;
		bool is_jumping = false;
		bool is_falling = false;
		bool is_landed = false;
		bool is_attacking = false;
		u32 attack_index_prev = 0;
		std::array<SafeUniquePtr<IState>, 5> attack_states_vec;
		bool is_dodging = false;
		bool is_damaged = false;
		Vector3 move_input = { 0,0,0 };
		bool can_climb = false;
		Vector3 climb_start_pos;
		Vector3 climb_end_pos;

	};
}

