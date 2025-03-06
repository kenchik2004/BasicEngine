#include "precompile.h"
#include "SampleObject.h"
#include "System/Components/RigidBody.h"
#include <System/Components/Collider.h>
#include <System/Components/CapsuleCollider.h>
#include <System/Components/BoxCollider.h>
#include <System/Components/ModelRenderer.h>



float anim_time = 0;
float anim_time_max = 0;
int anim = -1;
int SampleObject::Init()
{
	transform->SetPosition(float3(0, 0, 5));
	auto model = AddComponent<ModelRenderer>();
	model->rot = Quaternion(DEG2RAD(180), Vector3(0, 1, 0));
	model->Load("data/model.mv1", "model_1");
	model->scale = { 0.01f,0.01f,0.01f };
	model->SetAnimation("data/anim_walk.mv1", "walk", 1);
	model->SetAnimation("data/anim_stand.mv1", "stand", 1);
	model->SetAnimation("data/anim_punch.mv1", "punch", 0);
	model->PlayAnimationNoSame("stand");

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

	if (!GetComponent<ModelRenderer>()->IsPlaying())
		if (fist.lock())
			fist.lock()->RemoveThisComponent();
	if (Input::PushHitKey(KEY_INPUT_SPACE) && !fist.lock()) {
		GetComponent<ModelRenderer>()->PlayAnimationNoSame("punch");
		fist = AddComponent<CapsuleCollider>();
		fist.lock()->is_trigger = true;
		fist.lock()->height = 0.3f;
		fist.lock()->radius = 0.1f;
		fist.lock()->AttachToModel(10);
		fist.lock()->rotation = Quaternion(DEG2RAD(90), Vector3(0, 0, 1));
		fist.lock()->position = Vector3(0, 0, 0);
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
		SetCameraPositionAndTargetAndUpVec(float3(transform->position+Vector3(0,1.6f,0)-transform->AxisZ()*0.05f), float3(transform->position + Vector3(0, 1.6f, 0) + transform->AxisZ()), float3(transform->AxisY()));
}
void SampleObject::PrePhysics()
{

}

void SampleObject::PostPhysics()
{

}


void SampleObject::DebugDraw()
{

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
	color = YELLOW;
	if (hit_info.hit_collision->owner)
		hit_info.hit_collision->GetRigidBody()->SetVelocity(transform->AxisZ() * 10);
}



