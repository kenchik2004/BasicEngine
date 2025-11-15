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

	public:
		Player* player = nullptr;
		bool is_jumping = false;
		bool is_falling = false;
		bool is_attacking = false;
		bool fly = false;
		Vector3 move_input = { 0,0,0 };
		bool can_climb = false;
		Vector3 climb_start_pos;
		Vector3 climb_end_pos;

	};
}

