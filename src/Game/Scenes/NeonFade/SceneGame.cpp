#include "SceneGame.h"

#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
#include "Game/Objects/NeonFade/PoliceCar.h"

#include "Game/Managers/LightManager.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"

namespace NeonFade {
	//1ビルの縦横...100m×100m
	std::array<Vector3, 4> buildings = {
		Vector3(0.0f,0.0f,-300.0f),
		Vector3(101.0f,0.0f,-300.0f),
		Vector3(202.0f,0.0f,-300.0f),
		Vector3(303.0f,0.0f,-300.0f),
	};
	struct Trns {
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;
		Vector3 loc_pos;
		Quaternion loc_rot;
		Vector3 loc_scale;
	};
	std::vector<Trns> test = {
		{
			{0,0,0},
			Quaternion(0,0,0,1),
			{1,1,1},
			{0,0,0},
			Quaternion(0,0,0,1),
			{1,1,1}
		},
		{
			{0,1,0},
			Quaternion(DEG2RAD(30),{0,1,0}),
			{1,1,1},
			{0,1,0},
			Quaternion(DEG2RAD(90),{0,1,0}),
			{1,1,1},
		},
		{
			{0,2,0},
			Quaternion(DEG2RAD(60),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},

		{
			{0,3,0},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},

		{
			{0,4,0},
			Quaternion(DEG2RAD(120),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},
		{
			{0,5,0},
			Quaternion(DEG2RAD(150),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},
		{
			{0,6,0},
			Quaternion(DEG2RAD(180),{1,0,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},
		{
			{0,7,0},
			Quaternion(DEG2RAD(210),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},
		{
			{0,8,0},
			Quaternion(DEG2RAD(240),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},
		{
			{0,9,0},
			Quaternion(DEG2RAD(270),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},
		{
			{0,10,0},
			Quaternion(DEG2RAD(300),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		},
		{
			{0,11,0},
			Quaternion(DEG2RAD(330),{0,1,0}),
			{1,1,1},
			{0,0,1},
			Quaternion(DEG2RAD(90),{1,0,0}),
			{1,1,1},
		}

	};
	void SceneGame::Load()
	{

		loading_status = LOADING_STATUS::LOADING;
		ModelManager::LoadAsModel(u8"data/player/model.mv1", "player_model");
		ModelManager::LoadAsAnimation(u8"data/player/anim_stand.mv1", "idle");
		ModelManager::LoadAsAnimation(u8"data/player/anim_walk.mv1", "walk");
		ModelManager::LoadAsAnimation(u8"data/player/anim_run.mv1", "run");
		ModelManager::LoadAsAnimation(u8"data/player/anim_sprint.mv1", "sprint");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump.mv1", "jump");
		ModelManager::LoadAsAnimation(u8"data/player/anim_fall_loop.mv1", "fall_loop");
		ModelManager::LoadAsAnimation(u8"data/player/anim_combo.mv1", "combat_combo");
		ModelManager::LoadAsAnimation(u8"data/player/anim_climb.mv1", "climb");
		ModelManager::LoadAsAnimation(u8"data/player/anim_jump_attack.mv1", "jump_attack");
		ModelManager::LoadAsAnimation(u8"data/player/anim_back_flip.mv1", "back_flip");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/Ground.mv1", "stage");
		ModelManager::LoadAsModel(u8"data/Stage/Buildings/building-01_UV.mv1", "building");
		ModelManager::LoadAsModel(u8"data/Stage/megapolis/road_.mv1", "high-way");
		ModelManager::LoadAsModel(u8"data/car/police.mv1", "police");


		ModelManager::LoadAsModel(u8"data/enemy/model.mv1", "enemy_model");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_idle.mv1", "enemy_idle");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_damage.mv1", "enemy_damage");
		ModelManager::LoadAsAnimation(u8"data/enemy/anim_down.mv1", "enemy_down");
		AudioManager::Load(u8"data/Sound/siren.mp3", "siren");
		//TextureManager::Load(u8"data/Stage/megapolis/road.fbm/GT_AL_Dark Panel 2A_Roughness.png", "road_mat0_roughness");
		//TextureManager::Load(u8"data/Stage/megapolis/road.fbm/GT_AL_Dark Panel 2A_Metallic.png", "road_mat0_metallic");
		//TextureManager::Load(u8"data/Stage/megapolis/road.fbm/GT_AL_Dark Panel 1A_Roughness.png", "road_mat2_roughness");
		//TextureManager::Load(u8"data/Stage/megapolis/road.fbm/GT_AL_Dark Panel 1A_Metallic.png", "road_mat2_metallic");
		//TextureManager::Load(u8"data/Stage/megapolis/road.fbm/GT_AL_Glass 1_Roughness.png", "road_mat4_roughness");
		//TextureManager::Load(u8"data/Stage/megapolis/road.fbm/GT_AL_Glass 1_Metallic.png", "road_mat4_metallic");



		CheckForLoading();
	}

	int SceneGame::Init()
	{
		if (!CheckForLoading())
			return 0;
		physics_timescale = 2.0f;
		auto shadowmap = SceneManager::Object::Create<ShadowMapObject>("ShadowMap");
		shadowmap->SetCascadeCount(4);
		shadowmap->SetShadowMapSize(2048);
		shadowmap->SetLightDirection({ 0, -8, 5 });
		auto player_ = SceneManager::Object::Create<Player>(u8"プレイヤー");
		player_->transform->scale = { 0.05f,0.05f,0.05f };
		player_->transform->position = { 0,30,-20 };

		auto light_manager = SceneManager::Object::Create<LightManager>(u8"ライトマネージャー");
		light_manager->AddLight(LightType::Directional, { 0,0,0 }, { 20,20,100 }, 0, 0, { 0,-8,5 });

		for (u32 i = 0; i < buildings.size(); ++i)
			for (u32 j = 0; j < 20; ++j) {
				Vector3 rand_start = Vector3(-5, 20, -5);
				Vector3 rand_end = Vector3(5, 100, 5);
				Vector3 rand_pos = Random::Position(rand_start, rand_end);
				Vector3 offset = Vector3(rand_pos.x, 0, rand_pos.z);
				rand_pos += offset.getNormalized() * 60;
				rand_pos += buildings[i];
				light_manager->AddLight(LightType::Point, rand_pos, Random::Color({ 0,0,100 }, { 100,100,100 }), 50.0f, 4.0f);

			}

		if constexpr (true) {

			auto ground = SceneManager::Object::Create<GameObject>("Ground");
			ground->AddComponent<ModelRenderer>()->SetModel("stage");
			ground->AddComponent<RigidBody>();
			ground->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);
			ground->transform->scale = { 50,50,50 };
			ground->transform->position = { 0,0,0 };
		}
		camera = SceneManager::Object::Create<CameraObject>();
		camera->transform->position = { 0,10,10 };
		camera->transform->SetAxisZ({ 0,-0.75f,-1.0f });
		camera->camera->render_type = Camera::RenderType::Deferred;
		camera->camera->camera_far = 1000.0f;
		camera->AddComponent<AudioListener>();

		player_->player_camera = camera;
		player = player_;
		for (u32 i = 0; i < buildings.size(); ++i)
		{
			auto building = SceneManager::Object::Create<GameObject>("Building" + std::to_string(i));
			building->AddComponent<ModelRenderer>()->SetModel("building");
			building->transform->scale = { 2.0f,2.0f,2.0f };
			building->transform->position = buildings[i];
			auto rb = building->AddComponent<RigidBody>();
			rb->ChangeToStatic();
			//rb->mass = 0.01f;
			auto collision = building->AddComponent<BoxCollider>();
			collision->extension = { 100.0f, 200.0f, 100.0f };
			collision->position = { 0,100.0f,0 };
			collision->SetLayer(Collider::Layer::Terrain);
		}
		if constexpr (true)
		{
			auto high_way = SceneManager::Object::Create<GameObject>("HighWay");
			auto mod = high_way->AddComponent<ModelRenderer>();
			mod->SetModel("high-way");
			//auto mat = mod->GetMaterial(0);
			//mat->SetTexture(TextureManager::Get("road_mat0_roughness"), Material::TextureType::Roughness);
			//mat->SetTexture(TextureManager::Get("road_mat0_metallic"), Material::TextureType::Metalic);
			//auto mat2 = mod->GetMaterial(2);
			//mat2->SetTexture(TextureManager::Get("road_mat2_roughness"), Material::TextureType::Roughness);
			//mat2->SetTexture(TextureManager::Get("road_mat2_metallic"), Material::TextureType::Metalic);
			//auto mat4 = mod->GetMaterial(4);
			//mat4->SetTexture(TextureManager::Get("road_mat4_roughness"), Material::TextureType::Roughness);
			//mat4->SetTexture(TextureManager::Get("road_mat4_metallic"), Material::TextureType::Metalic);
			
			high_way->transform->position = { 0,2,0 };
			high_way->transform->scale = { 3.0f,3.0f,3.0f };
			high_way->AddComponent<RigidBody>();
			high_way->AddComponent<MeshCollider>()->SetLayer(Collider::Layer::Terrain);

		}
		for (u8 i = 0; i < 5; i++) {
			auto enem = SceneManager::Object::Create<Enemy>(u8"敵");
			enem->transform->scale = { 0.05f,0.05f,0.05f };
			enem->transform->position = { (float)i,60,-25 };
			//enem->transform->position = { i * 0.5f,40,90 };
		}
		constexpr float gravity_factor = -9.81f * 5;
		GetPhysicsScene()->setGravity({ 0,gravity_factor,0 });
		for (u32 i = 0; i < 5; ++i)
			SceneManager::Object::Create<PoliceCar>(u8"police_car")->SetPointOnpath(100 * i);

		return Super::Init();
	}

	void SceneGame::Update()
	{
		if (!CheckForLoading())
			return;




	}

	void SceneGame::PreDraw()
	{

	}

	void SceneGame::LateDebugDraw()
	{
		for (u32 i = 0; i < test.size(); i++) {
			auto& elem = test[i];
			//行列を使わずワールド座標の計算
			if (i == 0) {
				elem.loc_pos = elem.position;
				elem.loc_rot = elem.rotation;
				elem.loc_scale = elem.scale;
			}
			else {
				auto parent = test[i - 1];
				elem.loc_pos = { (parent.rotation.getBasisVector0() * parent.scale.x).dot(elem.position - parent.position),
					(parent.rotation.getBasisVector1() * parent.scale.y).dot(elem.position - parent.position),
					(parent.rotation.getBasisVector2() * parent.scale.z).dot(elem.position - parent.position)
				};

				elem.loc_rot = parent.rotation.getConjugate() * elem.rotation;
				elem.loc_scale = parent.scale / elem.scale;
				elem.loc_rot.normalize();

				elem.position = parent.rotation.rotate(parent.scale.multiply(elem.loc_pos)) + parent.position;
				elem.rotation = parent.rotation * elem.loc_rot;
				elem.rotation.normalize();
				elem.scale = elem.loc_scale.multiply(parent.scale);
			}

		}
		Vector3 offset = { 0,30,-30 };
		for (auto& elem : test) {
			DrawLine3D(cast(elem.position + offset), cast(offset + elem.position + elem.rotation.getBasisVector0()), Color::RED);
			DrawLine3D(cast(elem.position + offset), cast(offset + elem.position + elem.rotation.getBasisVector1()), Color::GREEN);
			DrawLine3D(cast(elem.position + offset), cast(offset + elem.position + elem.rotation.getBasisVector2()), Color::BLUE);
		}
	}
	void SceneGame::OnLateDrawFinish()
	{

		printfDx("%.2f fps\n", Time::GetDrawFPS());
		printfDx("%.2f update_fps\n", Time::GetFPS());
		Vector3 player_pos = player->transform->position;
		printfDx("Player Pos X:%.2f Y:%.2f Z:%.2f\n", player_pos.x, player_pos.y, player_pos.z);
		//DrawBoxAA(0, 50, sample_count_timer * 50.0f, 80, Color::RED, true);

	}

	void SceneGame::Exit()
	{
	}

	bool SceneGame::CheckForLoading()
	{
		if (loading_status == LOADING_STATUS::LOADED)
			return true;
		loading_status = ModelManager::GetLoadingCount() == 0 ? LOADING_STATUS::LOADED : LOADING_STATUS::LOADING;
		if (loading_status == LOADING_STATUS::LOADED)
			Init();
		return loading_status == LOADING_STATUS::LOADED;
	}

}