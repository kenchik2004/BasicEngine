#include "precompile.h"
#include "SampleObject.h"

int SampleObject::Init()
{
	transform->SetPosition(float3(0, 5, -5));
	SetCameraPositionAndTargetAndUpVec(float3(transform->position), float3(transform->position + transform->AxisZ()), float3(transform->AxisY()));
	return 0;
}

void SampleObject::Update()
{
	float speed = 3;
	if (Input::CheckHitKey(KEY_INPUT_LSHIFT))
		speed *= 2;
	if (Input::CheckHitKey(KEY_INPUT_W))
		transform->position += transform->AxisZ() * speed * Time::UnscaledDeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_S))
		transform->position -= transform->AxisZ() * speed * Time::UnscaledDeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_A))
		transform->position -= transform->AxisX() * speed * Time::UnscaledDeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_D))
		transform->position += transform->AxisX() * speed * Time::UnscaledDeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_SPACE))
		transform->position += transform->AxisY() * speed * Time::UnscaledDeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_LCONTROL))
		transform->position -= transform->AxisY() * speed * Time::UnscaledDeltaTime();


	if (Input::CheckHitKey(KEY_INPUT_UP))
		transform->AddRotation(float3(-3 * Time::UnscaledDeltaTime(), 0, 0));
	if (Input::CheckHitKey(KEY_INPUT_DOWN))
		transform->AddRotation(float3(3 * Time::UnscaledDeltaTime(), 0, 0));
	if (Input::CheckHitKey(KEY_INPUT_LEFT))
		transform->AddRotation(float3(0, -3 * Time::UnscaledDeltaTime(), 0));
	if (Input::CheckHitKey(KEY_INPUT_RIGHT))
		transform->AddRotation(float3(0, 3 * Time::UnscaledDeltaTime(), 0));
	if (Input::CheckHitKey(KEY_INPUT_Q))
		transform->AddRotation(float3(0, 0, 3 * Time::UnscaledDeltaTime()));
	if (Input::CheckHitKey(KEY_INPUT_E))
		transform->AddRotation(float3(0, 0, -3 * Time::UnscaledDeltaTime()));
}

void SampleObject::Draw()
{

}

void SampleObject::PreDraw()
{
	SetCameraPositionAndTargetAndUpVec(float3(transform->position), float3(transform->position + transform->AxisZ()), float3(transform->AxisY()));
}

void SampleObject::DebugDraw()
{
}

int SampleMovingObject::Init()
{
	transform->position = float3(0, 0, 0);
	elapsed = GetRand(DEG2RAD(360));
	return 0;
}

void SampleMovingObject::Update()
{
	elapsed += Time::DeltaTime();
	transform->position.x = sinf(elapsed) * 5;
	transform->position.z = cosf(elapsed) * 5;
	transform->rotation *= (Quaternion(Time::DeltaTime() * 3, Vector3(0, 1, 0)));
}

void SampleMovingObject::Draw()
{
	DrawSphere3D(float3(transform->position), 1, 32, GetColor(0, 255, 0), GetColor(0, 0, 0), true);
}

void SampleMovingObject::DebugDraw()
{
	float3 pos = transform->position;
	float3 scale = transform->scale;

	DrawSphere3D(pos, 1, 12, GetColor(0, 255, 0), GetColor(0, 0, 0), false);
	DrawLine3D(pos, (pos + transform->AxisX()), GetColor(0, 255, 0));
	DrawLine3D(pos, (pos + transform->AxisY()), GetColor(255, 0, 0));
	DrawLine3D(pos, (pos + transform->AxisZ()), GetColor(0, 0, 255));
}
