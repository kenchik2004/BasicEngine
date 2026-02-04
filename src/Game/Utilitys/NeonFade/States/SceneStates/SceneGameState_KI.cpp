#include "precompile.h"
#include "SceneGameState_KI.h"
#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Utilitys/NeonFade/StateMachines/SceneGameStateMachine.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"
#include "System/Objects/CameraObject.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Components/PlayerController.h"
#include "Game/Components/PlayerCameraMachine.h"

namespace NeonFade {

	SceneGameState_KI::SceneGameState_KI(SceneGame* owner_scene_)
		:ISceneState(static_cast<Scene*>(owner_scene_))
	{
		owner_scene_game = owner_scene_;
		camera_path = make_safe_unique<CatmullRomPath>();
		std::vector<Vector3> points = {
			Vector3(0, 180, 150),
			Vector3(-50, 150, 100),
			Vector3(0, 120, 50),
			Vector3(50, 90, 100),
			Vector3(0, 60, 150),
			Vector3(-50, 30, 100),
			Vector3(0, 10, 70),
		};
		camera_path->SetPoints(points, false);
		scene_camera = owner_scene_game->camera;
		message_text = owner_scene_game->ui_texts["txt_message"]->GetComponent<Text>();

	}
	void SceneGameState_KI::OnEnter(ISceneStateMachine* machine)
	{
		auto controller = owner_scene_game->player->pl_controller.lock();
		auto cam_machine = owner_scene_game->player->player_camera_machine.lock();
		controller->Sleep();
		cam_machine->Sleep();
		owner_scene_game->text_comp->SetText(u8"KI State");
		exit_timer = 0;
		movie_camera = SceneManager::Object::Create<CameraObject>(u8"movie_camera_KI");
		movie_camera->camera->render_type = Camera::RenderType::Deferred;
	}
	void SceneGameState_KI::OnExit(ISceneStateMachine* machine)
	{
		scene_camera->camera->SetCurrentCamera();
		SceneManager::Object::Destroy(movie_camera.lock());
		owner_scene_game->player->pl_controller->WakeUp();
		owner_scene_game->player->player_camera_machine->WakeUp();
		message_text->Sleep();
	}
	void SceneGameState_KI::Update(ISceneStateMachine* machine, float dt)
	{
		exit_timer += dt;

		Vector3 pos;
		Quaternion rot;
		camera_path->Evaluate((exit_timer / EXIT_TIME) * camera_path->GetTotalLength(), pos, rot);
		movie_camera->transform->position = pos;
		Vector3 look_dir = owner_scene_game->player->transform->position - pos;
		look_dir.normalize();
		movie_camera->transform->SetAxisZ(look_dir);
		message_text->TextColor().a = sinf(Time::GetTimeFromStart() * 3.0f) * 0.5f + 0.5f;

		if (exit_timer > EXIT_TIME) {
			machine->ChangeState("Show");
			return;
		}
	}
}
