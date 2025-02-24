#include "precompile.h"
#include "SceneSample.h"
#include "SampleObject.h"

void SceneSample::Load()
{
}

int SceneSample::Init()
{
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 1, 0, 0),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f))
	);
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(1, 0, 0, 7),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f))
	);
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(-1, 0, 0, 7),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f))
	);
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 0, 1, 7),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f))
	);
	GetPhysicsScene()->addActor(*physx::PxCreatePlane(
		*PhysicsManager::GetPhysicsInstance(), physx::PxPlane(0, 0, -1, 7),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f))
	);
	obj = SceneManager::Object::Create<SampleObject>();

	SetCameraPositionAndTarget_UpVecY(float3(5, 5, -5), float3(0, 1, 0));

	return 0;
}

void SceneSample::Update()
{

	if (Input::CheckHitKey(KEY_INPUT_F))
		Time::SetFPSMAX(Time::GetFPSMAX() + 2);

	if (Input::CheckHitKey(KEY_INPUT_G))
		Time::SetFPSMAX(Time::GetFPSMAX() - 2);

	if (Input::PushHitKey(KEY_INPUT_RETURN))
		obj->camera = !obj->camera;
	if (Input::PushHitKey(KEY_INPUT_RSHIFT)) {
		auto obj_ = SceneManager::Object::Create<SampleObject3>();
		obj_->transform->AddRotation(float3(0, 0, 90));
		obj_->velocity = float3(0, -1000, 0);
		obj_->transform->SetPosition(float3(0, 100, 0));

	}
}

void SceneSample::Draw()
{
	if (!obj->camera)
		SetCameraPositionAndTarget_UpVecY(float3(0, 3, -3), float3(0, 1, 0));
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

