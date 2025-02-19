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



