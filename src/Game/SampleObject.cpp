#include "precompile.h"
#include "SampleObject.h"

int SampleObject::Init()
{
	transform->SetPosition(float3(0, 5, -5));
	SetCameraPositionAndAngle(transform->position, transform->rotation.x, transform->rotation.y, transform->rotation.z);
	return 0;
}

void SampleObject::Update()
{
	float speed = 3;
	if (Input::CheckHitKey(KEY_INPUT_LSHIFT))
		speed *= 2;
	if (Input::CheckHitKey(KEY_INPUT_W))
		transform->position += transform->AxisZ() * speed * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_S))
		transform->position -= transform->AxisZ() * speed * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_A))
		transform->position -= transform->AxisX() * speed * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_D))
		transform->position += transform->AxisX() * speed * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_SPACE))
		transform->position += transform->AxisY() * speed * Time::DeltaTime();

	if (Input::CheckHitKey(KEY_INPUT_LCONTROL))
		transform->position -= transform->AxisY() * speed * Time::DeltaTime();


	if (Input::CheckHitKey(KEY_INPUT_UP))
		transform->rotation.x -= 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_DOWN))
		transform->rotation.x += 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_LEFT))
		transform->rotation.y -= 3 * Time::DeltaTime();
	if (Input::CheckHitKey(KEY_INPUT_RIGHT))
		transform->rotation.y += 3 * Time::DeltaTime();
}

void SampleObject::Draw()
{

}

void SampleObject::PreDraw()
{
	SetCameraPositionAndAngle(transform->position, transform->rotation.x, transform->rotation.y, transform->rotation.z);
}

void SampleObject::DebugDraw()
{
}

int SampleMovingObject::Init()
{
	transform->position = float3(0, 0, 0);
	return 0;
}

void SampleMovingObject::Update()
{
	elapsed += Time::DeltaTime();
	transform->position.x = sinf(elapsed) * 5;
	transform->position.z = cosf(elapsed) * 5;
	transform->rotation.y += Time::DeltaTime() * 3;
}

void SampleMovingObject::Draw()
{
	DrawSphere3D(transform->position, 1, 32, GetColor(0, 255, 0), GetColor(0, 0, 0), true);
}

void SampleMovingObject::DebugDraw()
{
	float3 pos = transform->position;
	float3 scale = transform->scale;

	DrawSphere3D(transform->position, 1, 12, GetColor(0, 255, 0), GetColor(0, 0, 0), false);
	DrawLine3D(pos, (pos + transform->AxisX()), GetColor(0, 255, 0));
	DrawLine3D(pos, (pos + transform->AxisY()), GetColor(255, 0, 0));
	DrawLine3D(pos, (pos + transform->AxisZ()), GetColor(0, 0, 255));
}
