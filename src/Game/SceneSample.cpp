#include "precompile.h"
#include "SceneSample.h"
#include "SceneSample2.h"
#include "SampleObject.h"
#include <System/Components/RigidBody.h>
#include <System/Components/BoxCollider.h>
#include <System/Components/ModelRenderer.h>
#include <System/Components/MeshCollider.h>


void SceneSample::Load()
{
}
physx::PxShape* shape = nullptr;

int SceneSample::Init()
{

#if 0
	auto box = SceneManager::Object::Create<Object>();
	box->transform->position = Vector3(0, 0, 0);
	box->AddComponent<RigidBody>()->use_gravity = false;
	auto box_col = box->AddComponent<BoxCollider>();
	box_col->extension = { 200,0.1f,200 };
	box_col->GetRigidBody()->is_kinematic = true;
#endif


	obj = SceneManager::Object::Create<SampleObject>();
	obj.lock()->transform->rotation = Quaternion(DEG2RAD(180), {0,1,0});
	for (int j = 0; j < 0; j++)
		for (int i = 0; i < 10; i++) {
			auto a = SceneManager::Object::Create<Object>("Box");
			auto mod = a->AddComponent<ModelRenderer>();
			mod->Load("data/cube.mv1", "cube");
			mod->scale = { 0.005f,0.005f,0.005f };
			mod->pos = { 0,-1000.5f,0 };
			a->transform->position = Vector3(i - 6.0f, j + 0.5f, 6.0f - i);
			a->transform->rotation = Quaternion(DEG2RAD(45), Vector3(0, 1, 0));
			auto rb = a->AddComponent<RigidBody>();
			rb->mass = 0.01f;
			//rb->SetMassCenter({ 0,0.5f,0 });
			auto col = a->AddComponent<BoxCollider>();
			//col->position = { 0,0.5f,0 };

		}

	obj2 = SceneManager::Object::Create<Object>();
	obj2.lock()->tag = ObjBase::TAG::Stage;
	obj2.lock()->transform->position = { 0,-1,0 };
	obj2.lock()->transform->scale = { 0.01f,0.01f,0.01f };
	obj2.lock()->AddComponent<RigidBody>();
	obj2.lock()->AddComponent<ModelRenderer>()->Load("data/Stage/stage00.mv1");
	auto mod_col = obj2.lock()->AddComponent<MeshCollider>();

	return 0;
}

void SceneSample::Update()
{

	if (Input::CheckHitKey(KEY_INPUT_F))
		Time::SetFPSMAX(Time::GetFPSMAX() + 2);

	if (Input::CheckHitKey(KEY_INPUT_G))
		Time::SetFPSMAX(Time::GetFPSMAX() - 2);

	if (Input::PushHitKey(KEY_INPUT_RETURN))
		SceneManager::Load<SceneSample2>();

}

void SceneSample::PreDraw()
{
	SetCameraPositionAndTarget_UpVecY(float3(0, 5, -5), float3(0, 1, 0));
}

void SceneSample::Draw()
{
	//	SetCameraPositionAndTarget_UpVecY(float3(3, 3, -3), float3(0, 1, 0));
	for (int x = -7; x <= 7; x++)
	{
		DrawLine3D(float3(x, 0, -7), float3(x, 0, 7), GetColor(255, 255, 255));
		for (int z = -7; z <= 7; z++)
			DrawLine3D(float3(-7, 0, z), float3(7, 0, z), GetColor(255, 255, 255));
	}

	DrawLine3D(float3(0, 0, 0), float3(1, 0, 0), GetColor(0, 255, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 1, 0), GetColor(255, 0, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 0, 1), GetColor(0, 0, 255));
}

void SceneSample::LateDraw()
{
	DrawBox(0, 0, 300, 50, GetColor(128, 0, 0), true);
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS : %.2f / %.2f", Time::GetFPSMAX(), Time::GetFPS());
	DrawFormatString(0, 24, GetColor(255, 255, 255), "Objects : %d", SceneManager::Object::GetArray<ObjBase>().size());
}
void SceneSample::DebugDraw()
{
	DrawLine3D(float3(0, 0, 0), float3(5, 0, 0), GetColor(0, 255, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 5, 0), GetColor(255, 0, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 0, 5), GetColor(0, 0, 255));

	auto obj_ = SceneManager::Object::Get<SampleObject>();
	float3 pos = obj_->transform->position;
	float3 vec = obj_->transform->AxisX();
	float3 vec2 = obj_->transform->AxisY();
	float3 vec3 = obj_->transform->AxisZ();


	DrawLine3D(pos, pos + vec * 5, GetColor(255, 255, 0));
	DrawLine3D(pos, pos + vec2 * 5, GetColor(255, 255, 0));
	DrawLine3D(pos, pos + vec3 * 5, GetColor(255, 255, 0));



}
void SceneSample::LateDebugDraw()
{
	DrawBox(0, 0, 300, 50, GetColor(128, 0, 0), true);
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS : %.2f / %.2f", Time::GetFPSMAX(), Time::GetFPS());
	DrawFormatString(0, 24, GetColor(255, 255, 255), "Objects : %d", SceneManager::Object::GetArray<ObjBase>().size());
}
void SceneSample::UnLoad()
{
}

