#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade {
	class TitlePlayer;
	class TitlePlayerStartSpawnState :
		public IState
	{
	public:
		TitlePlayerStartSpawnState(TitlePlayer* owner_);
		void OnEnter(IStateMachine* machine);
		void OnExit(IStateMachine* machine);
		void Update(IStateMachine* machine, float dt);

	private:
		TitlePlayer* owner_player = nullptr;
		float spawn_timer = 0.0f;
		Animator* anim = nullptr;
		ModelRenderer* model = nullptr;
		std::vector<Material*> spawn_mats = { nullptr };
		std::vector<Material*> default_mats = { nullptr };
		ShaderPs* spawn_ps = nullptr;
	};

}
