#include "SampleMovingScene.h"
#include "Game/Objects/Sample/SampleMovingCharacter.h"
#include "Game/Managers/LightManager.h"



namespace Sample {
	Material* mat = nullptr;
	class GroundObject :public GameObject {
	private:
		ModelRendererWP model;
	public:
		USING_SUPER(GroundObject);
		int Init() override {

			model = AddComponent<ModelRenderer>();
			model->SetModel("stage");
			AddComponent<RigidBody>();
			AddComponent<MeshCollider>();
			return Super::Init();
		}
		void Draw() override {
			auto shader_ps = model->GetMaterial(0)->GetPixelShader();
			Color color = { 1,1,1,1 };
			shader_ps->SetValue<Color>("SampleCB.samplecb_color", &color);
		}

	};
	void SampleMovingScene::Load()
	{
		loading_status = LOADING_STATUS::LOADING;
		ModelManager::LoadAsModel(u8"data/player/model.mv1", "player_model");
		ModelManager::LoadAsAnimation(u8"data/player/anim_stand.mv1", "idle");
		ModelManager::LoadAsAnimation(u8"data/player/anim_walk.mv1", "walk");
		ModelManager::LoadAsAnimation(u8"data/player/anim_run.mv1", "run");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump.mv1", "jump");
		ModelManager::LoadAsModel(u8"data/Stage/SwordBout/Stage00.mv1", "stage");
		TextureManager::Load("data/old_paper.png", "old_paper");
		TextureManager::Load("data/FX.png", "fx_texture");
		MaterialManager::LoadPixelShader("data/shader/sample_ps_2d.fx", "sample_ps_2d");


		CheckForLoading();
	}
	int SampleMovingScene::Init()
	{
		if (!CheckForLoading())
			return Super::Init();




		auto light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
		light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 10,10,10 }, 0, 0, { 0,-8,5 });

		auto shadowmap = SceneManager::Object::Create<ShadowMapObject>("ShadowMap");
		shadowmap->SetCascadeCount(4);
		shadowmap->SetShadowMapSize(2048);
		shadowmap->SetLightDirection({ 0, -8, 5 });
		auto player = SceneManager::Object::Create<SampleMovingCharacter>("Player");
		player->transform->position = { 0,10,0 };
		auto ground = SceneManager::Object::Create<GroundObject>("Ground");
		ground->transform->scale = { 0.05f,0.05f,0.05f };
		ground->transform->position = { 0,-10,0 };
		auto cam = SceneManager::Object::Create<CameraObject>();
		cam->transform->position = { 0,7,5 };
		cam->transform->SetAxisZ({ 0,-0.5f,-1.0f });
		//cam->camera->render_type = Camera::RenderType::Deferred;
		//cam->transform->SetParent(player->transform);
		camera = cam;
		this->player = player;


		//----------テクスチャレンダラーのテスト
		mat = MaterialManager::CreateMaterial("override_2d");
		mat->SetTexture(TextureManager::CloneByName("old_paper"), Material::TextureType::Diffuse);
		mat->SetTexture(TextureManager::CloneByName("fx_texture"), Material::TextureType::Normal);
		auto ui = SceneManager::Object::Create<UIObject>();
		ui->transform->position = { 0,0,0 };
		ui->transform->scale = { 1280*0.5f,720*0.5f,1 };
		ui->CanvasAnchorType() = UIObject::ANCHOR_TYPE::LEFT_TOP;
		ui->AnchorType() = UIObject::ANCHOR_TYPE::LEFT_TOP;
		auto render = ui->AddComponent<ImageRenderer>();
		mat->SetShaderPs(MaterialManager::LoadPixelShader("data/shader/sample_ps_2d.fx", "sample_ps_2d"));
		render->SetMaterial(mat);
		//-----------

		return Super::Init();
	}

	void SampleMovingScene::Update()
	{
		if (!CheckForLoading())
			return;
		if (Input::GetKey(KeyCode::Up))
			camera_distance -= 2.0f * Time::DeltaTime();
		if (Input::GetKey(KeyCode::Down))
			camera_distance += 2.0f * Time::DeltaTime();
		if (Input::GetKey(KeyCode::Left))
			camera_rot_y -= 30.0f * Time::DeltaTime();
		if (Input::GetKey(KeyCode::Right))
			camera_rot_y += 30.0f * Time::DeltaTime();
	}


	void SampleMovingScene::PreDraw()
	{

		if (!CheckForLoading())
			return;
		camera->transform->position = player->transform->position + Quaternion(DEG2RAD(camera_rot_y), { 0,1,0 }).rotate(Vector3(0, 1, 1.5f).getNormalized()) * camera_distance;
		camera->transform->SetAxisZ(player->transform->position - camera->transform->position + Vector3(0, 5, 0));
	}

	void SampleMovingScene::LateDraw()
	{

		if (!CheckForLoading()) {
			DrawFormatString(100, 100, 0xff0000, "Now Loading...");
			return;
		}
		DrawFormatString(0, 0, 0xffffff, "FPS:%.2f", Time::GetDrawFPS());
		float sampleparam1 = Time::GetTimeFromStart();
		float sampleparam2 = Input::GetKey(KeyCode::P) ? 2.0f : 0.0f;
		static float speed = 1.0f;
		if (Input::GetKey(KeyCode::O)) {
			speed += Time::DeltaTime();
		}
		if (Input::GetKey(KeyCode::L)) {
			speed -= Time::DeltaTime();
			if (speed < 0.0f)
				speed = 0.0f;
		}
		Vector3 sampleparams = { sampleparam1,sampleparam2,speed };
		mat->GetPixelShader()->SetValue("SampleCB.sampleparam1", &sampleparams);
	}

	void SampleMovingScene::Exit()
	{
	}

	bool SampleMovingScene::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADING) {
			loading_status = (ModelManager::GetLoadingCount() == 0 && AudioManager::GetLoadingCount() == 0) ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
			if (loading_status == LOADING_STATUS::LOADED) {
				Init();
				return true;
			}
			return false;
		}
		return true;

	}
}