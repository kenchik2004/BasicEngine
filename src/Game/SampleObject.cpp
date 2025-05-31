#include "precompile.h"
#include "SampleObject.h"
#include "System/Components/RigidBody.h"
#include <System/Components/Collider.h>
#include <System/Components/CapsuleCollider.h>
#include <System/Components/BoxCollider.h>
#include <System/Components/ModelRenderer.h>
#include <System/Components/Animator.h>
#include <Game/SampleAttack.h>
#include <System\Components\AudioListener.h>


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
	auto stand = ModelManager::CloneAnimByName("stand", 1);
	auto&& lambda = [=]() {
		animator->Play("punch");
		stand->ResetCallBack("p"); };
	stand->SetCallBack(std::move(lambda), 60, "p");
	animator->SetAnimation(stand);
	animator->SetAnimation("walk", 1);
	animator->SetAnimation("punch", 0);
	animator->PlayIfNoSame("stand");

	AddComponent<RigidBody>()->freeze_rotation = { 1, 1, 1 };
	auto col = AddComponent<CapsuleCollider>();
	col->height = 1.0f;
	col->radius = 0.3f;
	col->rotation = Quaternion(DEG2RAD(90), Vector3(0, 0, 1));
	col->position = Vector3(0.5f, 0, 0);
	AddComponent<AudioListener>();

	return 0;
}

void SampleObject::Update()
{
	anim_time += Time::DeltaTime() * 10;
	float3 velocity_factor = float3(0, 0, 0);
	if (Input::GetKey(KeyCode::W))
		velocity_factor += transform->AxisZ();
	if (Input::GetKey(KeyCode::S))
		velocity_factor -= transform->AxisZ();
	if (Input::GetKey(KeyCode::D))
		velocity_factor += transform->AxisX();
	if (Input::GetKey(KeyCode::A))
		velocity_factor -= transform->AxisX();
	if (Input::GetKey(KeyCode::Left))
		transform->AddRotation(Vector3(0, -45 * Time::DeltaTime(), 0));
	if (Input::GetKey(KeyCode::Right))
		transform->AddRotation(Vector3(0, 45 * Time::DeltaTime(), 0));

	velocity_factor = velocity_factor.normalized() * 10;
	velocity_factor.y = GetComponent<RigidBody>()->velocity.y;
	if (Input::GetKeyDown(KeyCode::Space))
		velocity_factor.y = 5;
	GetComponent<RigidBody>()->velocity = velocity_factor;

	if (Input::GetKeyDown(KeyCode::Space) && !GetComponent<SampleAttack>()) {
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
	SetCameraPositionAndTargetAndUpVec(float3(transform->position + Vector3(0, 1.6f, 0) - transform->AxisZ() * 0.05f), float3(transform->position + Vector3(0, 1.6f, 0) + transform->AxisZ()), float3(transform->AxisY()));
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



