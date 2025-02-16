#include "precompile.h"
#include "SceneSample.h"

void SceneSample::Load()
{
}

int SceneSample::Init()
{
	obj = SceneManager::Object::Create<Object>();

	obj->transform->position = float3(0, 1.5f, 0);
	obj->transform->scale = float3(3, 3, 3);
	return 0;
}

void SceneSample::Update()
{
	if (Input::CheckHitKey(KEY_INPUT_W))
		obj->transform->position.z += 3 * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_S))
		obj->transform->position.z -= 3 * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_A))
		obj->transform->position.x -= 3 * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_D))
		obj->transform->position.x += 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_UP))
		obj->transform->rotation.x -= 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_DOWN))
		obj->transform->rotation.x += 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_LEFT))
		obj->transform->rotation.y -= 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_RIGHT))
		obj->transform->rotation.y += 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_LSHIFT))
		obj->transform->rotation.z -= 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_LCONTROL))
		obj->transform->rotation.z += 3 * Time::DeltaTime();

}

void SceneSample::Draw()
{

}

void SceneSample::LateDraw()
{
}
void SceneSample::DebugDraw()
{
	SetCameraPositionAndTargetAndUpVec(VGet(0, 5, -10), VGet(0, 0, 0), VGet(0, 1, 0));
	float3 axis_x = obj->transform->AxisX();
	float3 axis_y = obj->transform->AxisY();
	float3 axis_z = obj->transform->AxisZ();
	float3 pos = obj->transform->position;
	float3 scale = obj->transform->scale;

	SetCameraPositionAndTargetAndUpVec(VGet(0, 5, -10), VGet(0, 0, 0), VGet(0, 1, 0));

	DrawLine3D(pos.VGet(), (pos + axis_x * 3).VGet(), GetColor(0, 255, 0));
	DrawLine3D(pos.VGet(), (pos + axis_y * 3).VGet(), GetColor(255, 0, 0));
	DrawLine3D(pos.VGet(), (pos + axis_z * 3).VGet(), GetColor(0, 0, 255));
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS : %f / %f", Time::GetFPSMAX(), Time::GetFPS());
}
void SceneSample::UnLoad()
{
}

