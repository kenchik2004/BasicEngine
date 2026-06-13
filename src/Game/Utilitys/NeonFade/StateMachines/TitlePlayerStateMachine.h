#pragma once
#include "IStateMachine.h"

namespace NeonFade {

	class TitlePlayer;
	class TitlePlayerStateMachine :
		public IStateMachine
	{
	public:
		TitlePlayerStateMachine(TitlePlayer* owner_);

	private:
		TitlePlayer* owner_player;
	};

}
