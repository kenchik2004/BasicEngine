#include "precompile.h"
#include "SampleObject.h"
#include "System/Components/RigidBody.h"
#include <System/Components/Collider.h>
#include <System/Components/CapsuleCollider.h>
#include <System/Components/BoxCollider.h>
#include <System/Components/ModelRenderer.h>
#include <System/Components/Animator.h>
#include <Game/SampleAttack.h>



float anim_time = 0;
float anim_time_max = 0;
int anim = -1;
int SampleObject::Init()
{
	transform->SetPosition(float3(0, 0, 5));
	auto model = AddComponent<ModelRenderer>();
	model->SetModel("man");
	model->rot = Quaternion(DEG2RAD(180), Vector3(0, 1, 0));
	model->scale = { 0.01f,0.01f,0.01f };
	auto animator = AddComponent<Animator>();
	animator->SetAnimation("walk", 1);
	animator->SetAnimation("stand", 1);
	animator->SetAnimation("punch", 0);
	animator->PlayIfNoSame("stand");

	AddComponent<RigidBody>()->freeze_rotation = { 1, 1, 1 };
	auto col = AddComponent<CapsuleCollider>();
	col->height = 1.0f;
	col->radius = 0.3f;
	col->rotation = Quaternion(DEG2RAD(90), Vector3(0, 0, 1));
	col->position = Vector3(0.5f, 0, 0);


	return 0;
}

void SampleObject::Update()
{
	anim_time += Time::DeltaTime() * 10;
	float3 velocity_factor = float3(0, 0, 0);
	if (Input::CheckHitKey(KEY_INPUT_W))
		velocity_factor += transform->AxisZ();
	if (Input::CheckHitKey(KEY_INPUT_S))
		velocity_factor -= transform->AxisZ();
	if (Input::CheckHitKey(KEY_INPUT_D))
		velocity_factor += transform->AxisX();
	if (Input::CheckHitKey(KEY_INPUT_A))
		velocity_factor -= transform->AxisX();
	if (Input::CheckHitKey(KEY_INPUT_LEFT))
		transform->AddRotation(Quaternion(DEG2RAD(-120 * Time::DeltaTime()), Vector3(0, 1, 0)));
	if (Input::CheckHitKey(KEY_INPUT_RIGHT))
		transform->AddRotation(Quaternion(DEG2RAD(120 * Time::DeltaTime()), Vector3(0, 1, 0)));
	velocity_factor = velocity_factor.normalized() * 2.5f;
	velocity_factor.y = GetComponent<RigidBody>()->velocity.y;
	GetComponent<RigidBody>()->velocity = velocity_factor;

	if (Input::PushHitKey(KEY_INPUT_SPACE) && !GetComponent<SampleAttack>()) {
		AddComponent<SampleAttack>();
	}
	if (auto model = GetComponent<Animator>()) {
		if (!model->IsPlaying())
			model->PlayIfNoSame("stand", true);
	}

}

void SampleObject::LateUpdate()
{

}

void SampleObject::Draw()
{

}

void SampleObject::PreDraw()
{
	//SetCameraNearFar(0.1f, 3000.0f);
	//SetupCamera_Perspective(TO_RADIAN(45.0f));
	//SetCameraPositionAndTargetAndUpVec(float3(transform->position + Vector3(0, 1.6f, 0) - transform->AxisZ() * 0.05f), float3(transform->position + Vector3(0, 1.6f, 0) + transform->AxisZ()), float3(transform->AxisY()));
}
void SampleObject::PrePhysics()
{

}

void SampleObject::PostPhysics()
{

}


void SampleObject::DebugDraw()
{


	color = Color(255, 255, 0);
	DrawSphere3D(cast(transform->position), 0.5f, 8, color, color, true);

}

void SampleObject::Exit()
{


}

void SampleObject::OnCollisionEnter(const HitInfo& hit_info)
{
}

void SampleObject::OnTriggerEnter(const HitInfo& hit_info)
{
	color = Color::YELLOW;
	if (auto atk = GetComponent<SampleAttack>())
		atk->OnTriggerEnter(hit_info);
}



