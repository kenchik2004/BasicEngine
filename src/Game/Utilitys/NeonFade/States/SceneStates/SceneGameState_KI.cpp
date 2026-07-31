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

	class SpawnDiveEffectObj :public GameObject
	{
	public:
		USING_SUPER(SpawnDiveEffectObj);
		int Init() override
		{
			auto model = AddComponent<ModelRenderer>();
			//モデルの設定
			model->SetModel("dive_effect");
			//モデルのスケールを大きくする
			transform->scale = { MAX_SCALE, MAX_SCALE, MAX_SCALE };
			auto material = model->GetMaterial(0);
			//マテリアルのピクセルシェーダーをUVスクロール用のシェーダーに差し替える
			uv_scroll_shader = MaterialManager::LoadPixelShader(u8"data/shader/ps_dive_effect.fx", u8"ps_uv_scroll");
			material->SetShaderPs(uv_scroll_shader, true);
			//影は無効化
			model->SetCastShadow(false);

			return 0;
		}
		void Update() override {
			float dt = Time::DeltaTime();
			float scale_factor = SCALE_SPEED * dt;
			//サイズを小さくしていく
			if (transform->scale.x > 0.001f)
				transform->scale -= {scale_factor, scale_factor, scale_factor};
			//十分に小さくなったらオブジェクトを破棄する
			else
				SceneManager::Object::Destroy(shared_from_this());
		}
	private:
		ShaderPs* uv_scroll_shader = nullptr;
		static constexpr float MAX_SCALE = 0.5f;
		static constexpr float SCALE_SPEED = 0.3f;

	};


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
			Vector3(0, 260, 70),
			Vector3(0, 240, 80),
			Vector3(50, 220, 100),
			Vector3(0, 200, 150),
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
		auto player = owner_scene_game->player.lock();
		player->transform->position = { 0,220,100 };
		player->transform->rotation = Quaternion(physx::PxIdentity);
		player->player_camera->transform->position = { 0,10,10 };
		player->player_camera->transform->SetAxisZ({ 0,-0.75f,-1.0f });
		player->player_camera_machine->ResetCameraRot();
		player->pl_controller->GetStateMachine()->ChangeState("spawn");
		player->rb->velocity = Vector3(physx::PxZero);

		auto dive_effect_obj = SceneManager::Object::Create<SpawnDiveEffectObj>(u8"dive_effect");
		dive_effect_obj->transform->SetParent(player->transform);
		dive_effect_obj->transform->local_position = { 0, 0, 3.0f };
		//Y軸を下向き斜め45度くらいにする
		dive_effect_obj->transform->SetAxisY({ 0.0f,1.0f,-1.0f });

		exit_timer = 0;
		auto movie_camera_obj = SceneManager::Object::Get<CameraObject>(owner_scene_game->shared_from_this());
		movie_camera = movie_camera_obj->AddComponent<MovieCameraMachine>();
		movie_camera->SetTarget(owner_scene_game->player);
		movie_camera->SetPath(camera_path.get());
		owner_scene_game->audio_player->audio = AudioManager::CloneByName(u8"bgm");
		owner_scene_game->audio_player->loop = true;
		owner_scene_game->audio_player->Play();

		owner_scene_game->PauseGame(false);
		//ポーズを解除するとゲームタイマーが作動してしまうので、止めておく
		owner_scene_game->StopGameTimer();
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
	void SceneGameState_KI::DebugDraw()
	{
		if (camera_path) {
			std::array<Vector3, 256> path_points;
			for (int i = 0; i < path_points.size(); ++i) {
				Vector3 pos;
				Quaternion rot;
				camera_path->Evaluate(i / static_cast<float>(path_points.size() - 1) * camera_path->GetTotalLength(), pos, rot);
				path_points[i] = pos;
			}
			for (u32 i = 0; i < path_points.size() - 2; ++i) {
				DxLib::DrawLine3D(cast(path_points[i]), cast(path_points[i + 1]), Color::RED);
			}
		}
	}
}
