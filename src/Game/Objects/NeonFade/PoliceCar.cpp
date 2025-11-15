#include "precompile.h"
#include "PoliceCar.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"
#include "Game/Managers/LightManager.h"
#include "Game/Objects/NeonFade/Player.h"
namespace NeonFade
{
	static SafeUniquePtr<CatmullRomPath> path;
	SafeSharedPtr<AudioClip> siren_sound = nullptr;
	int PoliceCar::Init()
	{
		if (!path) {

			std::vector<Vector3> pts = {
				{ -152, 30, -29  },
				{ -107, 30, -24  },
				{ -38 , 30, -24  },
				{ 117 , 30, -24  },
				{ 163 , 30, -29  },
				{ 205 , 30, -67  },
				{ 205 , 30, -130 },
				{ 163 , 30, -168 },
				{ 117 , 30, -173 },
				{ -38 , 30, -173 },
				{ -107, 30, -173 },
				{ -152, 30, -168 },
				{ -194, 30, -130 },
				{ -194, 30, -67  },
			};
			path = make_safe_unique<CatmullRomPath>();
			path->SetPoints(pts, true);




		}
		name = u8"police_car";
		transform->scale = { 3.0f,3.0f,3.0f };

		rb = AddComponent<RigidBody>();
		rb->is_kinematic = true;

		{

			auto col = AddComponent<BoxCollider>();
			col->extension = { 10,6,25 };
			col->position = { 0,3,0 };
			col->SetHitGroup(Collider::Layer::Player | Collider::Layer::Enemy);
			col->SetLayer(Collider::Layer::Terrain);
			col = AddComponent<BoxCollider>();
			col->extension = { 10,3,10 };
			col->position = { 0,6,-3 };
			col->SetHitGroup(Collider::Layer::Player | Collider::Layer::Enemy);
			col->SetLayer(Collider::Layer::Terrain);
		}
		{
			auto trigger = AddComponent<BoxCollider>();
			trigger->extension = { 10,20,25 };
			trigger->position = { 0,13,0 };
			trigger->is_trigger = true;
			trigger->SetHitGroup(Collider::Layer::Player | Collider::Layer::Enemy);
			trigger->SetLayer(Collider::Layer::Vehicle);
		}
		{
			auto model = SceneManager::Object::Create<GameObject>(u8"police_model");
			model->transform->rotation = Quaternion(90 * RADIAN, { 0,1,0 });
			model->transform->SetParent(transform);
			model->transform->scale = { 5,5,5 };
			model->AddComponent<ModelRenderer>()->SetModel("police");
		}
		{
			audio_player = AddComponent<AudioPlayer>();
			static int cnt = 0;
			if (true) {
				siren_sound = AudioManager::CloneByName("siren");
				audio_player->SetAudio(siren_sound);
				siren_sound.reset();
				audio_player->loop = true;
				audio_player->radius = 500;
				audio_player->Play(Random::Range(0, 3));
			}
			cnt++;

		}
		{
			auto light_manager = SceneManager::Object::Get<LightManager>(GetScene());
			moving_light_red = make_safe_shared<PointLight>();
			moving_light_red->color = { 100,0,0,0 };
			moving_light_red->position = { 0,0,90 };
			moving_light_red->intensity = 1.0f;
			moving_light_red->range = 50.0f;
			light_manager->AddLight(moving_light_red);
			moving_light_blue = make_safe_shared<PointLight>();
			moving_light_blue->color = { 0,0,100,0 };
			moving_light_blue->position = { 0,0,90 };
			moving_light_blue->intensity = 1.0f;
			moving_light_blue->range = 25.0f;
			light_manager->AddLight(moving_light_blue);

		}
		return Super::Init();
	}

	void PoliceCar::Update()
	{

		t += Time::DeltaTime() * 100;

		moving_light_red->range = Random::Range(20, 30);
		moving_light_blue->range = Random::Range(20, 30);

		{
			Vector3 pos_;
			Quaternion rot_;
			path->Evaluate(t, pos_, rot_);
			Vector3 pos_delta = -transform->position;
			transform->position = pos_ - Vector3(0, 5.5f, 0);
			pos_delta += transform->position;
			transform->rotation = rot_;
			Vector3 police_x = transform->AxisX();
			moving_light_blue->position = pos_ + Vector3(0, 3, 0) + police_x * 3;
			moving_light_red->position = pos_ + Vector3(0, 3, 0) - police_x * 3;
			if (player_rideon)
				rb->velocity = { 0,0,0 };
			else
				rb->velocity = transform->AxisZ() * 100;

			for (auto& rideon : rideon_objs) {
				rideon->transform->position += pos_delta;

			}
		}
	}

	void PoliceCar::Exit()
	{
		auto light_manager = SceneManager::Object::Get<LightManager>(GetScene());
		if (light_manager) {
			light_manager->RemoveLight(moving_light_red);
			light_manager->RemoveLight(moving_light_blue);
		}
		moving_light_red.reset();
		moving_light_blue.reset();
	}
	void PoliceCar::OnTriggerEnter(const HitInfo& hit_info)
	{
		//もしプレイヤーが乗ってきたら、乗せる(自分の子供にする)
		auto player = hit_info.hit_collision->owner;
		if (player->GetComponent<Collider>()->GetLayer() == Collider::Layer::Player) {
			player_rideon = true;
		}
		rideon_objs.push_back(player);

	}

	void PoliceCar::OnTriggerExit(const HitInfo& hit_info)
	{
		//もしプレイヤーが降りたら、親をリセットする
		auto player = hit_info.hit_collision->owner;
		if (player->GetComponent<Collider>()->GetLayer() == Collider::Layer::Player) {
			player_rideon = false;
		}
		rideon_objs.erase(std::find(rideon_objs.begin(), rideon_objs.end(), player));
	}
}
