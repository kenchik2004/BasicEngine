#include "PlayerDodgeState.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/PlayerController.h"

namespace NeonFade
{
	PlayerDodgeState::PlayerDodgeState(Player* owner_) :
		IState(static_cast<GameObject*>(owner_))
	{
		player = owner_;
		animator = player->animator.lock().get();
		rb = player->rb.lock().get();
		std::function<bool()> dodge_to_idle = [this]() {
			return dodge_timer >= DODGE_TIME;
			};
		RegisterChangeRequest("fall", dodge_to_idle, 0);
	}
	void PlayerDodgeState::OnEnter(IStateMachine* machine)
	{
		animator->Play("back_flip", false, 0.0f, 0.05f, true);
		dodge_timer = 0.0f;
		Vector3 dodge_dir = static_cast<PlayerStateMachine*>(machine)->move_input;
		if (dodge_dir.magnitudeSquared() <= 1e-6f * 1e-6f) {
			dodge_dir = -player->transform->AxisZ();
		}
		else {
			dodge_dir.normalize();
		}
		dodge_dir = ProjectOnPlane(dodge_dir, { 0,1,0 }) * -1;

		player->transform->SetAxisZ(dodge_dir.getNormalized(), { 0,1,0 });
		dodge_dir *= DODGE_SPEED;
		dodge_dir.y = 20; // 少しジャンプさせる
		rb->velocity = dodge_dir;
	}
	void PlayerDodgeState::OnExit(IStateMachine* machine)
	{
	}
	void PlayerDodgeState::Update(IStateMachine* machine, float dt)
	{
		dodge_timer += dt;
	}
}