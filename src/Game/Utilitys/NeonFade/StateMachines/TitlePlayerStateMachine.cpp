#include "TitlePlayerStateMachine.h"
#include "Game/Objects/NeonFade/TitlePlayer.h"
#include "Game/Utilitys/NeonFade/States/TitlePlayerStates/TitlePlayerStartSpawnState.h"
#include "Game/Utilitys/NeonFade/States/TitlePlayerStates/TitlePlayerStartFlipState.h"
#include "Game/Utilitys/NeonFade/States/TitlePlayerStates/TitlePlayerStartDiveState.h"

namespace NeonFade {
	TitlePlayerStateMachine::TitlePlayerStateMachine(TitlePlayer* owner_)
		:IStateMachine(static_cast<GameObject*>(owner_))
	{
		owner_player = owner_;

		auto start_spawn_state = make_safe_unique<TitlePlayerStartSpawnState>(owner_player);
		AddState("start_spawn", std::move(start_spawn_state));
		auto start_flip_state = make_safe_unique<TitlePlayerStartFlipState>(owner_player);
		AddState("player_flip", std::move(start_flip_state));
		auto start_dive_state = make_safe_unique<TitlePlayerStartDiveState>(owner_player);
		AddState("player_dive", std::move(start_dive_state));

		ChangeState("start_spawn");
	}
}