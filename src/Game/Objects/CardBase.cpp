#include "precompile.h"
#include "CardBase.h"
#include "System/Components/Camera.h"
namespace {
	int mytex = -1;

}
namespace RLyeh {
	int CardBase::Init()
	{

		model = AddComponent<ModelRenderer>();
		//TextureManager::Load(reinterpret_cast<const char*>(u8"data/カード/CardTexture.png"), "my_tex");
		//auto texture = SceneManager::GetCurrentScene()->GetCurrentCameraRef();
		if (mytex < 0)
			mytex = LoadGraph(reinterpret_cast<const char*>(u8"data/カード/CardTexture_Temp.png"));
		if (my_texture < 0)
			my_texture = mytex;
		ModelManager::LoadAsModel(reinterpret_cast<const char*>(u8"data/カード/Card.mv1"), "card");
		ModelManager::LoadAsModel(reinterpret_cast<const char*>(u8"data/Boss.mv1"), "character");
		model->SetModel("card");
		SetTexture(my_texture);
#if 1
		auto obj = SceneManager::Object::Create<Object>(GetScene());
		obj->transform->AddRotation({ 0,0,0 });
		transform->SetChild(obj->transform);
		{
			auto obj2 = SceneManager::Object::Create<Object>(GetScene());
			auto child_model2 = obj2->AddComponent<ModelRenderer>();
			child_model2->SetModel("character");
			obj2->transform->AddRotation({ 0,180,0 });
			obj2->transform->scale = { 0.01f,0.01f,0.01f };
			obj->transform->SetChild(obj2->transform);
			obj2->transform->SetChild(transform);
		}

#endif
		transform->position = { 3,0,0 };
		start_pos = transform->position;
		return 0;
	}

	void CardBase::Update()
	{

		auto cam = GetScene()->GetCurrentCameraRef()->owner;
		Vector3 target = Vector3(0, 4, 0);
		Quaternion target_rot = { 0,0,0,1 };
		if (cam) {
			target = cam->transform->position + cam->transform->AxisZ() * 4.5f;
			target_rot = cam->transform->rotation * Quaternion(DEG2RAD(-90), { 1,0,0 });
		}
		switch (mode)
		{
		case 0:
			delta = 0;
			break;
		case 1:
			transform->AddRotation({ 0,0, Time::DeltaTime() * 420 * 3 * (delta < 1.3f ? 1.3f - delta : 0) });
			transform->position = Lerp(start_pos, target, delta);
			delta += Time::DeltaTime();

			if ((target - transform->position).magnitude() <= 0.05f) {
				transform->position = target;
				mode++;
				delta = 0;
			}
			break;
		case 2:
			delta += Time::DeltaTime();
			{
				Quaternion qx(DEG2RAD(-15), Vector3(1, 0, 0));
				Quaternion qy(DEG2RAD(10), Vector3(0, 1, 0));
				Quaternion qz(DEG2RAD(10), Vector3(0, 0, 1));
				transform->rotation = Slerp(transform->rotation, target_rot * qx * qy * qz, 0.1f * Time::DeltaTime() * 60);
			}
			if (delta >= 1.0f) {
				if (auto child = transform->GetChild(0).lock()) {
					transform->ResetChild(child);
				}
				transform->ResetParent();
				delta = 0;
				mode++;
			}
			break;
		case 3:
			transform->rotation = Slerp(transform->rotation, target_rot, Time::DeltaTime() * 5);
			transform->position = target;
			delta += Time::DeltaTime();
			if (delta > 0.7f) {
				//transform->rotation = target_rot;
				mode++;
				delta = 0;
			}
			break;
		case 4:
			transform->position = Lerp(target, { 0,0.5f,0 }, delta);
			delta += Time::DeltaTime() * 5;
			transform->rotation = Slerp(transform->rotation, { 0,0,0,1 }, delta);
			if (delta > 1) {

				mode++;
			}
			break;
		case 5:
			break;
			transform->position = { 0,0.5f,0 };
			if (Input::GetKeyDown(KeyCode::Return)) {
				transform->position = Random::Position({ -5,0.1f,-5 }, { 5,0.1f,5 });
				start_pos = transform->position;
			}
		default:
			break;
		}
		if (Input::GetKeyDown(KeyCode::Return))
			++mode < 5 ? 0 : mode = 0;

		if (Input::GetKey(KeyCode::U)) {
			transform->GetChild(0)->local_position += { Time::DeltaTime(), 0, 0};
		}
		if (Input::GetKey(KeyCode::I)) {
			transform->GetChild(0)->local_position -= {Time::DeltaTime(), 0, 0 };
		}
		if (Input::GetKey(KeyCode::O)) {
			transform->GetChild(0)->local_scale = { 0,0,0 };
		}
		if (Input::GetKey(KeyCode::P)) {
			transform->GetChild(0)->local_scale = { 1,1,1 };
		}
	}

	void CardBase::Exit()
	{
		DeleteGraph(mytex);
	}

	void CardBase::LateDraw()
	{
		Vector3 p = transform->position;
		Vector3 p_ = transform->local_position;
		DrawFormatString(10, 50, GetColor(255, 255, 255), "global_position:%.2f,%.2f,%.2f", p.x, p.y, p.z);
		DrawFormatString(10, 70, GetColor(255, 255, 255), "local_position:%.2f,%.2f,%.2f", p_.x, p_.y, p_.z);

	}

	void CardBase::SetTexture(int tex)
	{
		MV1SetTextureGraphHandle(model->GetModelHandle(), 0, tex, false);
	}

}