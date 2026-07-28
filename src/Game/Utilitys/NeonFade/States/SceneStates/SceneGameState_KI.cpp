//---------------------------------------------------------------------------
//! @file   SceneGameState_KI.cpp
//! @brief  SceneGameState_KIの実装。ゲームの起（序盤）シーン状態処理を行う
//---------------------------------------------------------------------------
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

	class MovieCameraMachine : public Component {
	public:
		USING_SUPER(CameraObject);
		int Init() override {
			int ret = Super::Init();
			camera_object = SafeStaticCast<CameraObject>(owner.lock());
			camera_object->camera->render_type = Camera::RenderType::Deferred;
			return ret;
		}
		void PreDraw() override {
			if (!path || !target)
				return;

			Vector3 pos;
			Quaternion rot;
			path->Evaluate(path_timer * path->GetTotalLength(), pos, rot);
			camera_object->transform->position = pos;
			Vector3 look_dir = target->transform->position - pos;
			look_dir.normalize();
			camera_object->transform->SetAxisZ(look_dir);
		}
		void SetPath(CatmullRomPath* path_) {
			path = path_;
		}
		void SetTarget(GameObjectWP target_) {
			target = target_;
		}
		void SetPathTimer(float timer) {
			path_timer = timer;
		}
	private:
		CameraObjectWP camera_object = nullptr;
		GameObjectWP target = nullptr;
		CatmullRomPath* path = nullptr;
		float path_timer = 0.0f;
	};

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
		timer_text = owner_scene_game->ui_texts["txt_time"];

	}
	void SceneGameState_KI::OnEnter(ISceneStateMachine* machine)
	{
		auto controller = owner_scene_game->player->pl_controller.lock();
		auto cam_machine = owner_scene_game->player->player_camera_machine.lock();
		controller->SetIgnoreInput(true);
		cam_machine->Sleep();
		message_text->SetText(u8"全員倒せ!");
		message_text->SetFontSize(170);
		message_text->WakeUp();
		message_text->ResetDrawChar();
		timer_text->Sleep();
		owner_scene_game->text_comp->SetText(u8"KI State");
		owner_scene_game->player->transform->position = { 0,220,100 };
		owner_scene_game->player->transform->rotation = Quaternion(physx::PxIdentity);
		owner_scene_game->player->player_camera->transform->position = { 0,10,10 };
		owner_scene_game->player->player_camera->transform->SetAxisZ({ 0,-0.75f,-1.0f });
		owner_scene_game->player->player_camera_machine->ResetCameraRot();
		owner_scene_game->player->pl_controller->GetStateMachine()->ChangeState("spawn");
		owner_scene_game->player->rb->velocity = Vector3(physx::PxZero);
		exit_timer = 0;
		auto movie_camera_obj = SceneManager::Object::Get<CameraObject>(owner_scene_game->shared_from_this());
		movie_camera = movie_camera_obj->AddComponent<MovieCameraMachine>();
		movie_camera->SetTarget(owner_scene_game->player);
		movie_camera->SetPath(camera_path.get());
		owner_scene_game->audio_player->audio = AudioManager::CloneByName(u8"bgm");
		owner_scene_game->audio_player->loop = true;
		owner_scene_game->audio_player->Play();

		owner_scene_game->PauseGame(false);
	}
	void SceneGameState_KI::OnExit(ISceneStateMachine* machine)
	{
		scene_camera->camera->SetCurrentCamera();
		if (movie_camera)
			movie_camera->RemoveThisComponent();
		owner_scene_game->player->pl_controller->SetIgnoreInput(false);
		owner_scene_game->player->player_camera_machine->WakeUp();
		owner_scene_game->player->player_camera_machine->Init();
		message_text->Sleep();
	}
	void SceneGameState_KI::Update(ISceneStateMachine* machine, float dt)
	{
		exit_timer += dt;

		movie_camera->SetPathTimer(exit_timer / EXIT_TIME);

		message_text->TextColor().a = sinf(Time::GetTimeFromStart() * 3.0f) * 0.5f + 0.5f;

		if (exit_timer > EXIT_TIME) {
			machine->ChangeState("Show");
			return;
		}
	}
}
