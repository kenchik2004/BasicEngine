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
#include "Game/Scenes/NeonFade/SceneGame.h"

#include <fstream>

namespace NeonFade
{

	std::vector<Vector3> pts = {
		{ 28.7075,8.64885,206.782},
		{ -100,10.5126,199.317 },
		{ -199.796,45.4285,199.157 },
		{ -100,24.7298,450.123 },
		{ 0.6576,18.4236,451.062 },
		{ 164.458,46.265,405.531 },
		{ 212.67,66.5275,386.228 },
		{ 212.67,65.8438,303.588 },
		{ 232.385,40.3841,204.081 },
		{ 120.578,12.3191,207.101 },

	};

	int PoliceCar::Init()
	{

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
		enem_crash_sound = AudioManager::CloneByName(u8"enem_crash");
		return Super::Init();
	}

	void PoliceCar::Update()
	{

		t += Time::DeltaTime() * 100;
		sound_cooltimer -= Time::DeltaTime();

		moving_light_red->range = Random::Range(0.01f, 50);
		moving_light_blue->range = Random::Range(0.01f, 50);
#ifdef _DEBUG
		static u32 idx = 0;
		const float mov_speed = 50.0f;
		// テンキーの8,2,4,6で前後左右に移動する
		Vector3 mov = { 0,0,0 };
		if (Input::GetKey(KeyCode::NumPad8))
			mov.z = 1;
		if (Input::GetKey(KeyCode::NumPad2))
			mov.z = -1;
		if (Input::GetKey(KeyCode::NumPad4))
			mov.x = -1;
		if (Input::GetKey(KeyCode::NumPad6))
			mov.x = 1;
		// テンキーの9,3で上下に移動する
		if (Input::GetKey(KeyCode::NumPad9))
			mov.y = 1;
		if (Input::GetKey(KeyCode::NumPad3))
			mov.y = -1;
		if (Input::GetKeyDown(KeyCode::LBracket))
			pts.push_back(pts.back() + Vector3(0, 10, 0));
		if (Input::GetKeyDown(KeyCode::RBracket)) {
			if (pts.size() > 2) {
				pts.pop_back();
				idx = idx % pts.size();
			}
		}
		if (Input::GetKeyDown(KeyCode::NumPad7))
			idx = (idx + 1) % pts.size();
		if (Input::GetKeyDown(KeyCode::NumPad1))
			idx = (idx + pts.size() - 1) % pts.size();

		// 移動量を正規化して、移動速度を掛け適用する
		mov = mov.getNormalized() * mov_speed;
		pts[idx] += mov * Time::DeltaTime();
		static float frame = 0.0f;
		frame += Time::DeltaTime();
		if (frame >= 0.5f) {
			path->SetPoints(pts, true);
			frame = 0.0f;
		}




#endif

		{
			Vector3 pos_;
			Quaternion rot_;
			path->Evaluate(t, pos_, rot_);
			Vector3 pos_delta = -transform->position;
			transform->position = pos_ - Vector3(0, 5.5f, 0);
			pos_delta += transform->position;
			transform->rotation = rot_;
			Vector3 police_x = transform->AxisX();
			Vector3 police_y = transform->AxisY();
			moving_light_blue->position = pos_ + police_y * 3.5f + police_x * 3;
			moving_light_red->position = pos_ + police_y * 3.5f - police_x * 3;
			if (player_rideon)
				rb->velocity = { 0,0,0 };
			else
				rb->velocity = pos_delta;

			for (auto& rideon : rideon_objs) {
				if (rideon)
					rideon->transform->position += pos_delta;

			}
		}
	}

	void PoliceCar::Exit()
	{
		auto light_manager = SceneManager::Object::Get<LightManager>(GetScene());
		std::unique_ptr<std::ofstream> debug_out;
		{
			debug_out = std::make_unique<std::ofstream>("data/police_car_path_debug.txt");
			for (auto& pt : pts) {
				(*debug_out) << "{" << pt.x << "," << pt.y << "," << pt.z << "}," << std::endl;
			}
		}
		if (light_manager) {
			light_manager->RemoveLight(moving_light_red);
			light_manager->RemoveLight(moving_light_blue);
		}
		moving_light_red.reset();
		moving_light_blue.reset();
	}
	void PoliceCar::OnCollisionEnter(const HitInfo& hit_info)
	{
		auto other = hit_info.hit_collision->owner;
		if (other->GetComponent<Collider>()->GetLayer() == Collider::Layer::Enemy) {
			Vector3 vec = other->transform->position - transform->position;
			vec.normalize();
			vec += Vector3(0, 0.5f, 0);
			auto enem = SafeDynamicCast<NeonFade::Enemy>(other.lock());
			enem->Damage(9999, true);	//即死ダメージ
			enem->Down(vec * 100);		//吹き飛ばし
			PlayCrashSound();
		}
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
			auto enem = SafeDynamicCast<NeonFade::Enemy>(other.lock());
			enem->Damage(9999, true);	//即死ダメージ
			enem->Down(vec * 100);		//吹き飛ばし
			PlayCrashSound();
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
	void PoliceCar::PlayCrashSound()
	{
		if (enem_crash_sound && sound_cooltimer <= 0.0f) {
			enem_crash_sound->PlayOneShot(SceneGame::GetSEVolume());
			sound_cooltimer = SOUND_COOLTIME;
		}
	}
}
