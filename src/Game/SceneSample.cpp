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
	obj = SceneManager::Object::Create<SampleObject>();


	return 0;
}

void SceneSample::Update()
{

	if (Input::CheckHitKey(KEY_INPUT_F))
		Time::SetFPSMAX(Time::GetFPSMAX() + 2);

	if (Input::CheckHitKey(KEY_INPUT_G))
		Time::SetFPSMAX(Time::GetFPSMAX() - 2);


}

void SceneSample::Draw()
{
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


void SceneSample::DebugDraw()
{




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

