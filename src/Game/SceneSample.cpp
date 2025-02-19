#include "precompile.h"
#include "SceneSample.h"
#include "SampleObject.h"

void SceneSample::Load()
{
}

int SceneSample::Init()
{
	obj = SceneManager::Object::Create<SampleObject>();
	SceneManager::Object::Create<SampleMovingObject>();
	for (int i = 0; i < 0; i++) {
		auto obj_ = SceneManager::Object::Create<SampleMovingObject>();
		for (int j = 0; j < 1; j++)
			obj_->AddComponent<Component>();
	}
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
}
void SceneSample::DebugDraw()
{
	DrawLine3D(float3(0, 0, 0), float3(5, 0, 0), GetColor(0, 255, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 5, 0), GetColor(255, 0, 0));
	DrawLine3D(float3(0, 0, 0), float3(0, 0, 5), GetColor(0, 0, 255));


	float3 vec = obj->transform->AxisX();
	float3 vec2 = obj->transform->AxisY();
	float3 vec3 = obj->transform->AxisZ();


	DrawLine3D(float3(0, 0, 0), vec * 5, GetColor(255, 255, 0));
	DrawLine3D(float3(0, 0, 0), vec2 * 5, GetColor(255, 255, 0));
	DrawLine3D(float3(0, 0, 0), vec3 * 5, GetColor(255, 255, 0));


	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS : %f / %f", Time::GetFPSMAX(), Time::GetFPS());
	DrawFormatString(0, 24, GetColor(255, 255, 255), "Objects : %d", SceneManager::Object::GetArray<ObjBase>().size());

}
void SceneSample::UnLoad()
{
}

