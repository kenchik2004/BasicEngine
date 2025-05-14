#include "precompile.h"
#include "SceneSample.h"
#include "SceneSample2.h"
#include "SampleObject.h"
#include <System/Components/RigidBody.h>
#include <System/Components/BoxCollider.h>
#include <System/Components/ModelRenderer.h>
#include <System/Components/MeshCollider.h>
#include <System/Components/ConvexMeshCollider.h>
#include "Game/DiceD4.h"
#include "Game/DiceD6.h"
#include "Game/DiceD8.h"
#include "Game/DiceD10.h"
#include "Game/DiceD12.h"
#include "Game/DiceD20.h"
#include "System/ModelManager.h"


void SceneSample::Load()
{
	ModelManager::LoadAsModel("data/dice/D4.mv1", "D4");
	ModelManager::LoadAsModel("data/dice/D6.mv1", "D6");
	ModelManager::LoadAsModel("data/dice/D8.mv1", "D8");
	ModelManager::LoadAsModel("data/dice/D10.mv1", "D10");
	ModelManager::LoadAsModel("data/dice/D12.mv1", "D12");
	ModelManager::LoadAsModel("data/dice/D20.mv1", "D20");
	ModelManager::LoadAsModel("data/Stage/stage00.mv1", "Stage");

	ModelManager::LoadAsModel("data/model.mv1", "man");
	ModelManager::LoadAsAnimation("data/anim_walk.mv1", "walk");
	ModelManager::LoadAsAnimation("data/anim_stand.mv1", "stand");
	ModelManager::LoadAsAnimation("data/anim_punch.mv1", "punch");
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
	obj.lock()->transform->rotation = Quaternion(DEG2RAD(135), { 0,1,0 });
	obj.lock()->transform->position = { -1,0,0 };
	handle_d4 = LoadGraph("data/dice/D4.png");
	handle_d6 = LoadGraph("data/dice/D6.png");
	handle_d8 = LoadGraph("data/dice/D8.png");
	handle_d10 = LoadGraph("data/dice/D10.png");
	handle_d12 = LoadGraph("data/dice/D12.png");
	handle_d20 = LoadGraph("data/dice/D20.png");
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 10; j++)
		{
			auto dice = SceneManager::Object::Create<DiceD4>(handle_d4);

			dice->transform->position = Vector3(i - 5.0f, 5.6f, j - 5.0f);
		}

	//plane = physx::PxCreatePlane(
	//	*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, 0),
	//	*Material::Concrete_Default);
	//GetPhysicsScene()->addActor(*plane);
	
	
	obj2 = SceneManager::Object::Create<Object>();
	obj2.lock()->tag = ObjBase::TAG::Stage;
	obj2.lock()->transform->position = { 0,-1,0 };
	obj2.lock()->transform->scale = { 0.01f,0.01f,0.01f };
	obj2.lock()->AddComponent<RigidBody>()->use_gravity = false;

	obj2.lock()->AddComponent<ModelRenderer>()->SetModel("Stage");
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
		SceneManager::Load<SceneSample>();

	if (Input::PushHitKey(KEY_INPUT_P)) {

		DrawString(SCREEN_W * 0.5f, SCREEN_H * 0.5f, "スキップ中", Color::CYAN);
		ScreenFlip();
		float delta = 1.0f / 50.0f;

		SceneManager::PrePhysics();
		for (float i = 0.0f; i < 10.0f; i += delta) {


			GetPhysicsScene()->simulate(delta);
			GetPhysicsScene()->fetchResults(true);
		}
		SceneManager::PostPhysics();

	}
}

void SceneSample::PreDraw()
{
	SetCameraPositionAndTarget_UpVecY(float3(0, 7, -10), float3(0, 1, 0));
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
void SceneSample::Exit()
{
	if(plane)
	plane->release();
	DeleteGraph(handle_d4);
	DeleteGraph(handle_d6);
	DeleteGraph(handle_d8);
	DeleteGraph(handle_d10);
	DeleteGraph(handle_d12);
	DeleteGraph(handle_d20);
}
void SceneSample::UnLoad()
{
}

