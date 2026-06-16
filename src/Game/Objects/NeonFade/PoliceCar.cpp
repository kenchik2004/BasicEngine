//---------------------------------------------------------------------------
//! @file   PoliceCar.cpp
//! @brief  PoliceCarオブジェクトの実装。パトカーオブジェクトのゲームロジックを管理する
//---------------------------------------------------------------------------
#include "precompile.h"
#include "PoliceCar.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"
#include "Game/Managers/LightManager.h"
#include "Game/Objects/NeonFade/Player.h"
#include "Game/Objects/NeonFade/Enemy.h"
namespace NeonFade
{
	SafeSharedPtr<AudioClip> siren_sound = nullptr;
	int PoliceCar::Init()
	{
		std::vector<Vector3> pts = {
			{ 0,	10, 530  },
			{ -100, 25, 530  },
			{ -200, 50, 480  },
			{ -100, 25, 430  },
			{ 0,	10, 430  },
			{ 100,	25, 430  },
			{ 200,	50, 480  },
			{ 100,	25, 530  },
		};
		path = make_safe_unique<CatmullRomPath>();
		path->SetPoints(pts, true);


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
			auto model = SceneManager::Object::Create<GameObject>(u8"police_car");
			model->transform->rotation = Quaternion(90 * RADIAN, { 0,1,0 });
			model->transform->SetParent(transform);
			model->transform->scale = { 5,5,5 };
			model->AddComponent<ModelRenderer>()->SetModel("police_car");
		}
		{
			auto light_manager = SceneManager::Object::Get<LightManager>(GetScene());

			static const Color light_color_red = { 1000,0,0,0 };
			static const Color light_color_blue = { 0,0,1000,0 };
			static const Vector3 light_pos = { 0,0,90 };
			static const float light_intensity = 0.02f;
			static const float light_range = 50.0f;

			{
				moving_light_red = make_safe_shared<PointLight>(light_pos, light_color_red, light_intensity, light_range);
				light_manager->AddLight(moving_light_red);
			}
			{
				moving_light_blue = make_safe_shared<PointLight>(light_pos, light_color_blue, light_intensity, light_range);
				light_manager->AddLight(moving_light_blue);
			}

		}
		return Super::Init();
	}

	void PoliceCar::Update()
	{

		t += Time::DeltaTime() * 100;

		moving_light_red->range = Random::Range(0.01f, 50);
		moving_light_blue->range = Random::Range(0.01f, 50);

		{
			Vector3 pos_;
			Quaternion rot_;
			path->Evaluate(t, pos_, rot_);
			Vector3 pos_delta = -transform->position;
			transform->position = pos_ - Vector3(0, 5.5f, 0);
			pos_delta += transform->position;
			transform->rotation = rot_;
			Vector3 police_x = transform->AxisX();
			moving_light_blue->position = pos_ + Vector3(0, 3.5f, 0) + police_x * 3;
			moving_light_red->position = pos_ + Vector3(0, 3.5f, 0) - police_x * 3;
			if (player_rideon)
				rb->velocity = { 0,0,0 };
			else
				rb->velocity = pos_delta;

			for (auto& rideon : rideon_objs) {
				if(rideon)
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
		auto other = hit_info.hit_collision->owner;
		if (other->GetComponent<Collider>()->GetLayer() == Collider::Layer::Player) {
			player_rideon = true;
		}
		if (other->GetComponent<Collider>()->GetLayer() == Collider::Layer::Enemy) {
			Vector3 vec = other->transform->position - transform->position;
			vec.normalize();
			vec += Vector3(0, 0.5f, 0);
			SafeDynamicCast<NeonFade::Enemy>(other.lock())->Down(vec*100);
		}
		rideon_objs.push_back(other);

	}

	void PoliceCar::OnTriggerExit(const HitInfo& hit_info)
	{
		//もしプレイヤーが降りたら、親をリセットする
		auto other = hit_info.hit_collision->owner;
		if (other->GetComponent<Collider>()->GetLayer() == Collider::Layer::Player) {
			player_rideon = false;
		}
		rideon_objs.erase(std::find(rideon_objs.begin(), rideon_objs.end(), other));
	}
	void PoliceCar::DebugDraw()
	{
		if (path) {
			std::array<Vector3, 256> path_points;
			for (int i = 0; i < path_points.size(); ++i) {
				Vector3 pos;
				Quaternion rot;
				path->Evaluate(i / static_cast<float>(path_points.size() - 1) * path->GetTotalLength(), pos, rot);
				path_points[i] = pos;
			}
			for (u32 i = 0; i < path_points.size() - 2; ++i) {
				DxLib::DrawLine3D(cast(path_points[i]), cast(path_points[i + 1]), Color::RED);
			}
		}
	}
}
