#include "precompile.h"
#include "SampleMovingCharacter.h"
#include "System/Components/ModelRenderer.h"
#include "System/Components/Animator.h"
#include "System/Components/RigidBody.h"
#include "System/Components/CapsuleCollider.h"
#include "System/Objects/CameraObject.h"

int SampleMovingCharacter::Init()
{
	my_model = AddComponent<ModelRenderer>();
	my_model->SetModel("player_model");
	my_animator = AddComponent<Animator>();
	my_animator->SetAnimation("idle", 1);
	my_animator->SetAnimation("walk", 1);
	my_animator->SetAnimation("run", 1);
	my_animator->SetAnimation("jump", 0);
	std::function on_jump = [this]() {
		my_rb->velocity.y = 20.0f;
		};
	std::function on_jump_top = [this]() {
		my_rb->velocity.y = -10.0f;
		};
	std::function on_jump_end = [this]() {
		is_jumping = false;
		};
	if (use_rigidbody) {
		my_animator->SetAnimationCallBack("jump", on_jump, 0, "on_jump");
		my_animator->SetAnimationCallBack("jump", on_jump_top, 40, "on_jump_top");
	}
	my_animator->SetAnimationCallBack("jump", on_jump_end, 80, "on_jump_end");


	if (use_rigidbody) {
		auto rb = AddComponent<RigidBody>();
		rb->freeze_rotation = { 1,1,1 };
		my_rb = rb;
		auto col = AddComponent<CapsuleCollider>();
		col->height = 7.5f;
		col->radius = 1.5f;
		col->position = { 5.4f,0,0 };
		col->rotation = Quaternion(DEG2RAD(90), { 0,0,1 });
	}


	my_animator->anim_blend_time = 0.2f;
	transform->scale = Vector3(0.05f, 0.05f, 0.05f);
	transform->position = { 0,20,0 };
	return 0;
}

void SampleMovingCharacter::Update()
{
	if (ignore_input || !use_rigidbody)
		return;
	Vector3 mov = { 0,0,0 };
	if (Input::GetKey(KeyCode::W))
	{
		mov.z += 1.0f;
	}
	if (Input::GetKey(KeyCode::S))
	{
		mov.z -= 1.0f;
	}
	if (Input::GetKey(KeyCode::A))
	{
		mov.x -= 1.0f;
	}
	if (Input::GetKey(KeyCode::D))
	{
		mov.x += 1.0f;
	}
	if (Input::GetKey(KeyCode::Space) && !is_jumping)
	{
		my_animator->Play("jump", false, 0.0f, 0.2f, true);
		is_jumping = true;
	}
	if (mov.magnitudeSquared() > 0.0f)
	{
		auto cam = GetScene()->GetCurrentCamera();
		auto cam_rot = cam->owner->transform->rotation;
		mov = cam_rot.rotate(mov);
		mov.y = 0;
		mov *= -1;

		mov = mov.getNormalized() * 20.0f;
		if (!is_jumping)
			my_animator->PlayIfNoSame("run", true);
		Vector3 forward = transform->AxisZ();

		float dot = forward.dot(mov.getNormalized());


		dot = physx::PxClamp(dot, -1.0f, 1.0f);
		if (dot <= -0.9f)
		{
			//正反対の方向を向いている場合、Lerpがうまく働かないので、少しずらす
			//決め打ちでx,zをずらすと特定方向で不自然になるので、y軸を基準に外積でずらす

			mov = mov.getNormalized().cross({ 0,1,0 }) * 20;
			dot = forward.dot(mov.getNormalized());
		}
		float angle = RAD2DEG(acosf(dot));
		float rot_angle = 720 / angle;
		transform->SetAxisZ(Lerp(transform->AxisZ(), mov.getNormalized(), rot_angle * Time::DeltaTime()));
		mov *= -1;
		mov.y = my_rb->velocity.y; // 重力成分を維持
		my_rb->velocity = mov;
	}
	else
	{
		if (!is_jumping)
			my_animator->PlayIfNoSame("idle", true);
		Vector3 vel = my_rb->velocity;
		my_rb->velocity = Vector3(0, vel.y, 0);
	}
}

void SampleMovingCharacter::Exit()
{
}
