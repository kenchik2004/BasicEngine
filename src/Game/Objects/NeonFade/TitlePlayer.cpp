#include "TitlePlayer.h"

#include "Game/Utilitys/NeonFade/StateMachines/TitlePlayerStateMachine.h"

namespace NeonFade {

	TitlePlayer::TitlePlayer() = default;
	TitlePlayer::~TitlePlayer() = default;


	int TitlePlayer::Init() {
		AddComponent<ModelRenderer>()->SetModel("player_model");
		auto anim = AddComponent<Animator>();
		anim->SetAnimation("player_sitting");
		anim->SetAnimation("player_dive");
		anim->SetAnimation("player_twist_flip");
		transform->scale = { 0.05f,0.05f,0.05f };

		state_machine = std::make_unique<TitlePlayerStateMachine>(this);

		return Super::Init();
	}
	void TitlePlayer::Update()
	{
		Super::Update();
		state_machine->Update(Time::UnscaledDeltaTime());
	}
	void TitlePlayer::DebugDraw()
	{
		Super::DebugDraw();
		state_machine->DebugDraw();
	}
}