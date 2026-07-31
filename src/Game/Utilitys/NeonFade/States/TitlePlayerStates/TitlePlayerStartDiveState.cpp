#include "TitlePlayerStartDiveState.h"

#include "Game/Scenes/NeonFade/SceneGame.h"
#include "Game/Objects/NeonFade/TitlePlayer.h"

namespace NeonFade {

	class DiveEffectObj :public GameObject
	{
	public:
		USING_SUPER(DiveEffectObj);
		int Init() override
		{
			auto model = AddComponent<ModelRenderer>();
			//モデルの設定
			model->SetModel("dive_effect");
			//モデルのスケールを小さくする
			transform->scale = { 0.001f,0.002f,0.001f };
			auto material = model->GetMaterial(0);
			//マテリアルのピクセルシェーダーをUVスクロール用のシェーダーに差し替える
			uv_scroll_shader = MaterialManager::LoadPixelShader(u8"data/shader/ps_dive_effect.fx", u8"ps_uv_scroll");
			material->SetShaderPs(uv_scroll_shader, true);
			//影は無効化
			model->SetCastShadow(false);

			//子オブジェクトとして、少し大きめのモデルを追加する
			//縁が悪い意味で目立っているようなので、フェードさせたいところ
			//しかし、BEは完全Defferedレンダリングで駆動するようにしてしまった
			//アルファブレンドが使えないので、逆に自己発光+Bloomで疑似ぼかしをかけることにする
			if constexpr (true) {

				auto child_obj = SceneManager::Object::Create<GameObject>(u8"dive_effect_child");
				child_obj->transform->SetParent(transform);
				child_obj->transform->local_scale = { 1.1f,1.1f,1.1f };
				auto child_model = child_obj->AddComponent<ModelRenderer>();
				child_model->SetModel("dive_effect");
				MV1SetMaterialEmiColor(child_model->GetModelHandle(), 0, GetColorF(3, 3, 3, 3));
				auto child_model_material = child_model->GetMaterial(0);
				child_model_material->SetShaderPs(uv_scroll_shader, true);

			}

			return 0;
		}
		void Update() override {
			float dt = Time::DeltaTime();
			float scale_factor = SCALE_SPEED * dt;
			//最大サイズに達していない場合はスケールを増加させる
			if (transform->scale.x < MAX_SCALE)
				transform->scale += {scale_factor, scale_factor * 2.0f, scale_factor};
		}
	private:
		ShaderPs* uv_scroll_shader = nullptr;
		static constexpr float MAX_SCALE = 0.5f;
		static constexpr float SCALE_SPEED = 0.3f;

	};


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
			Vector3(-140, -480,	84),
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
		auto camera_obj_ = SceneManager::Object::Get<CameraObject>();
		camera_obj = camera_obj_;
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

		if (dive_timer > EFFECT_START_TIME && !effect_obj) {


			auto  dive_effect = SceneManager::Object::Create<DiveEffectObj>(u8"DiveEffect");
			dive_effect->transform->SetParent(camera_obj->transform);
			dive_effect->transform->local_position = { 0.0f,0.0f,50.0f };
			dive_effect->transform->local_rotation = Quaternion(DEG2RAD(-90), { 1,0,0 });
			effect_obj = dive_effect;
		}
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