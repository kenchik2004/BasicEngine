#include "TitlePlayerStartDiveState.h"

#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/TitlePlayer.h"

namespace NeonFade {
	TitlePlayerStartDiveState::TitlePlayerStartDiveState(TitlePlayer* owner) :
		IState(owner)
	{
		owner_player = owner;
		model = owner_player->GetComponent<ModelRenderer>().get();
		anim = owner_player->GetComponent<Animator>().get();
		if (!model || !anim) {
			owner_player = nullptr;
			model = nullptr;
			anim = nullptr;
			return;
		}
		path = std::make_unique<CatmullRomPath>();
		std::vector<Vector3> control_points = {
			Vector3(0, 0, 0),
			Vector3(-20, -8,	12),
			Vector3(-40, -15,	24),
			Vector3(-60, -30,	36),
			Vector3(-80, -60,	48),
			Vector3(-100, -120,	60),
			Vector3(-110, -240,	72),
			Vector3(-120, -480,	84),
			Vector3(-130, -960,	96),
			Vector3(-140, -1400,108),
		};
		path->SetPoints(control_points, false);

	}
	void TitlePlayerStartDiveState::OnEnter(IStateMachine* machine)
	{
		if (!owner_player || !model || !anim)
			return;
		anim->Play(u8"player_dive", true, 0.0f, 0.8f);
		initial_position = owner_player->transform->position;
		initial_rotation = owner_player->transform->rotation;
		//ダイブ後の回転は、X軸に90度回転させたもの
		diving_rotation = initial_rotation * Quaternion(DEG2RAD(-90), Vector3(1, 0, 0));
		camera_obj = SceneManager::Object::Get<CameraObject>();
	}
	void TitlePlayerStartDiveState::OnExit(IStateMachine* machine)
	{}
	void TitlePlayerStartDiveState::Update(IStateMachine* machine, float dt)
	{
		dive_timer += dt * 0.5f;
		owner_player->transform->rotation = Slerp(initial_rotation, diving_rotation, min(dive_timer, DIVE_DURATION));
		next = (ModelManager::GetLoadingCount() + TextureManager::GetLoadingCount() + AudioManager::GetLoadingCount()) == 0;
		// ダイブ中の最小時間を経過しており、かつリソースのロードが完了している場合、ゲームシーンに遷移する
		if (next && dive_timer >= MINIMUM_DIVE_TIME)
			SceneManager::Load<SceneGame>();
		Vector3 next_player_pos = { 0.0f,0.0f,0.0f };
		Quaternion dummy_rot = { 0.0f,0.0f,0.0f,1.0f };
		Vector3 up = { -1.0f, 0.0f, 1.0f };
		path->Evaluate(dive_timer * path->GetTotalLength() * DIVE_SPEED, next_player_pos, dummy_rot, true, up);
		next_player_pos += initial_position;
		owner_player->transform->position = next_player_pos;
		if (camera_obj) {
			Vector3 cur_pos = camera_obj->transform->position;
			Vector3 target_pos = owner_player->transform->position;
			Vector3 new_cam_pos = target_pos + owner_player->transform->AxisZ() * 30 + owner_player->transform->AxisY() * 30;
			camera_obj->transform->position = Lerp(cur_pos, new_cam_pos, 0.1f);
			Vector3 look_dir = ((owner_player->transform->position + Vector3(0, 5, 0)) - camera_obj->transform->position).getNormalized();
			look_dir = Slerp(camera_obj->transform->AxisZ(), look_dir, 0.1f);
			float dot = look_dir.dot(Vector3(0, 1, 0));
			Vector3 up_dir = owner_player->transform->AxisY();

			camera_obj->transform->SetAxisZ(look_dir, up_dir);
		}

	}
	void TitlePlayerStartDiveState::DebugDraw()
	{
		if (path) {
			std::array<Vector3, 256> path_points;
			for (int i = 0; i < path_points.size(); ++i) {
				Vector3 pos;
				Quaternion rot;
				path->Evaluate(i / static_cast<float>(path_points.size() - 1) * path->GetTotalLength(), pos, rot);
				path_points[i] = pos + initial_position;
			}
			for (u32 i = 0; i < path_points.size() - 2; ++i) {
				DxLib::DrawLine3D(cast(path_points[i]), cast(path_points[i + 1]), Color::RED);
			}
		}
	}
}