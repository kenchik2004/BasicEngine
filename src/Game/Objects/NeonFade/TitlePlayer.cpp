#include "TitlePlayer.h"

#include "Game/Utilitys/NeonFade/StateMachines/TitlePlayerStateMachine.h"

namespace NeonFade {

	TitlePlayer::TitlePlayer() = default;
	TitlePlayer::~TitlePlayer() = default;

	Vector3 title_player_pos = { 363.218f,1526.004f,148.985f };
	Quaternion title_player_rot = Quaternion(2.02f, { 0,1,0 });

	int TitlePlayer::Init() {
		AddComponent<ModelRenderer>()->SetModel("player_model");
		auto anim = AddComponent<Animator>();
		anim->SetAnimation("player_sitting");
		anim->SetAnimation("player_dive");
		anim->SetAnimation("player_twist_flip");
		anim->Play("player_sitting", true);
		transform->position = title_player_pos;
		transform->rotation = title_player_rot;
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