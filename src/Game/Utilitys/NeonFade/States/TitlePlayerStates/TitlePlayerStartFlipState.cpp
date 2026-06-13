#include "TitlePlayerStartFlipState.h"

#include "Game/Scenes/NeonFade/SceneTitle.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/TitlePlayer.h"
#include "System/Objects/CameraObject.h"


namespace NeonFade {
	TitlePlayerStartFlipState::TitlePlayerStartFlipState(TitlePlayer* player_) :
		IState(player_)
	{
		owner_player = player_;
		model = owner_player->GetComponent<ModelRenderer>().get();
		anim = owner_player->GetComponent<Animator>().get();
		if (!model || !anim) {
			owner_player = nullptr;
			model = nullptr;
			anim = nullptr;
			return;
		}
		std::function<void()> call_back = [this]() {
			next = true;
			};
		anim->SetAnimationCallBack(u8"player_twist_flip", call_back, 42, u8"TitlePlayerStartFlipStateCallBack");

		std::function<bool()> next_func = [this]() {
			if (!anim || !owner_player)
				return false;
			return next;
			};
		RegisterChangeRequest(u8"player_dive", next_func, 0);
	}
	void TitlePlayerStartFlipState::OnEnter(IStateMachine* machine)
	{
		if (!owner_player || !model || !anim)
			return;
		anim->Play(u8"player_twist_flip", false);
		camera_obj = SceneManager::Object::Get<CameraObject>();
	}
	void TitlePlayerStartFlipState::OnExit(IStateMachine* machine)
	{}
	void TitlePlayerStartFlipState::Update(IStateMachine* machine, float dt)
	{
		owner_player->transform->position += owner_player->transform->AxisZ() * -dt * mov_speed;
		owner_player->transform->position += owner_player->transform->AxisY() * -dt * mov_speed*0.5f;
		if (camera_obj) {
			Vector3 cur_pos = camera_obj->transform->position;
			Vector3 target_pos = owner_player->transform->position;
			Vector3 new_cam_pos = target_pos + owner_player->transform->AxisZ() * 30 + owner_player->transform->AxisY() * 30;
			camera_obj->transform->position = Lerp(cur_pos, new_cam_pos, 0.05f);
			Vector3 look_dir = ((owner_player->transform->position + Vector3(0, 5, 0)) - camera_obj->transform->position).getNormalized();
			look_dir = Slerp(camera_obj->transform->AxisZ(), look_dir, 0.1f);

			Vector3 up_dir = owner_player->transform->AxisY();

			camera_obj->transform->SetAxisZ(look_dir, up_dir);
		}
	}
}