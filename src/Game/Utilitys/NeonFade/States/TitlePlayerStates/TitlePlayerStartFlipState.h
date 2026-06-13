#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

USING_PTR(CameraObject);
namespace NeonFade {

	class TitlePlayer;
	class TitlePlayerStartFlipState :
		public IState
	{
	public:
		TitlePlayerStartFlipState(TitlePlayer* player_);
		void OnEnter(IStateMachine* machine);
		void OnExit(IStateMachine* machine);
		void Update(IStateMachine* machine, float dt);

	private:
		TitlePlayer* owner_player = nullptr;
		Animator* anim = nullptr;
		ModelRenderer* model = nullptr;
		CameraObjectWP camera_obj = nullptr;
		static constexpr float mov_speed = 30.0f; // 前方への移動速度
		bool next = false;
	};

}
