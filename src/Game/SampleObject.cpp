#include "precompile.h"
#include "SampleObject.h"
#include "System/Components/RigidBody.h"
#include <System/Components/Collider.h>
#include <System/Components/ModelRenderer.h>

mat4x4 MV1GetFrameLocalWorldMatrix(int MHandle, int FrameIndex, bool is_physx) {
	mat4x4 mat = cast(MV1GetFrameLocalWorldMatrix(MHandle, FrameIndex));
	mat = CastPhysXMat(mat);
	return mat;
}

float anim_time = 0;
float anim_time_max = 0;
int anim = -1;
int SampleObject::Init()
{
	transform->SetPosition(float3(0, 0, 5));
	auto model = AddComponent<ModelRenderer>();
	model->Load("data/model.mv1", "model_1");
	model->scale = { 0.01f,0.01f,0.01f };
	model->SetAnimation("data/anim_walk.mv1", "walk", 1);
	model->SetAnimation("data/anim_stand.mv1", "stand", 1);
	model->PlayAnimationNoSame("walk", true);
	//int a = MV1AttachAnim(model, 0, anim, false);
	//anim_time_max = MV1GetAttachAnimTotalTime(model, 0);
	//AddComponent<RigidBody>()->is_a = true;
	{
		auto scene = SceneManager::GetCurrentScene();
		auto p_scene = scene->GetPhysicsScene();
		rigid_dynamic
			= PhysicsManager::GetPhysicsInstance()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
		rigid_dynamic->userData = new std::weak_ptr<ObjBase>(shared_from_this());
		// Œ`ó(Sphere)‚ðì¬
		my_shape
			= PhysicsManager::GetPhysicsInstance()->createShape(
				// Sphere‚Ì”¼Œa
				physx::PxCapsuleGeometry(0.5f, 1.0f),
				// –€ŽCŒW”‚Æ”½”­ŒW”‚ÌÝ’è
				*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f)
			);
		// Œ`ó‚ð•R‚Ã‚¯
		my_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		my_shape->setSimulationFilterData(physx::PxFilterData(1, 1, 0, 0));
		//my_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
		//my_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);

		rigid_dynamic->attachShape(*my_shape);
		//rigid_dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
		p_scene->addActor(*rigid_dynamic);
	}

	return 0;
}

void SampleObject::Update()
{
	anim_time += Time::DeltaTime() * 10;
	//if (anim_time >= anim_time_max)
	//	anim_time = 0;
	//MV1SetAttachAnimTime(model, 0, anim_time);
	float3 velocity_factor = float3(0, 0, 0);
	if (Input::CheckHitKey(KEY_INPUT_UP))
		velocity_factor += float3(0, 0, 1);
	if (Input::CheckHitKey(KEY_INPUT_DOWN))
		velocity_factor -= float3(0, 0, 1);
	if (Input::CheckHitKey(KEY_INPUT_RIGHT))
		velocity_factor += float3(1, 0, 0);
	if (Input::CheckHitKey(KEY_INPUT_LEFT))
		velocity_factor -= float3(1, 0, 0);
	velocity_factor = velocity_factor.normalized() * 2.5f;
	velocity_factor.y = velocity.y;
	velocity = velocity_factor;

	if (Input::CheckHitKey(KEY_INPUT_W))
		transform->AddRotation(float3(-30 * Time::UnscaledDeltaTime(), 0, 0));
	if (Input::CheckHitKey(KEY_INPUT_S))
		transform->AddRotation(float3(30 * Time::UnscaledDeltaTime(), 0, 0));
	if (Input::CheckHitKey(KEY_INPUT_A))
		transform->AddRotation(float3(0, -30 * Time::UnscaledDeltaTime(), 0));
	if (Input::CheckHitKey(KEY_INPUT_D))
		transform->AddRotation(float3(0, 30 * Time::UnscaledDeltaTime(), 0));
	if (Input::CheckHitKey(KEY_INPUT_Q))
		transform->AddRotation(float3(0, 0, 30 * Time::UnscaledDeltaTime()));
	if (Input::CheckHitKey(KEY_INPUT_E))
		transform->AddRotation(float3(0, 0, -30 * Time::UnscaledDeltaTime()));
	if (Input::CheckHitKey(KEY_INPUT_U))
		transform->SetAxisZ(float3(0, 0, 1));



}

void SampleObject::LateUpdate()
{

}

void SampleObject::Draw()
{


	//MV1DrawModel(model);
}

void SampleObject::PreDraw()
{
	if (camera)
		SetCameraPositionAndTargetAndUpVec(float3(transform->position), float3(transform->position + transform->AxisZ()), float3(transform->AxisY()));
}
void SampleObject::PrePhysics()
{
	auto model = GetComponent<ModelRenderer>();
	float3 frame_pos = MV1GetFramePosition(model->model.handle, 5);
	mat4x4 frame_mat = MV1GetFrameLocalWorldMatrix(model->model.handle, 5, true);
	physx::PxTransform t(frame_mat);
	frame_pos += t.q.rotate(float3(0, 0, 0)) - transform->position;
	Quaternion q = Inverse(transform->rotation);
	frame_pos = q.rotate(frame_pos);
	rigid_dynamic->detachShape(*my_shape);

	my_shape->setLocalPose(physx::PxTransform(frame_pos, q * t.q * Quaternion(DEG2RAD(90), Vector3(1, 0, 0))));


	rigid_dynamic->setGlobalPose(physx::PxTransform(transform->position, transform->rotation));

	rigid_dynamic->attachShape(*my_shape);
	rigid_dynamic->setLinearVelocity(velocity);
}

void SampleObject::PostPhysics()
{
	auto trns = rigid_dynamic->getGlobalPose();
	transform->position = trns.p;
	transform->rotation = trns.q;
	velocity = rigid_dynamic->getLinearVelocity();
}


void SampleObject::DebugDraw()
{
	physx::PxTransform trns = rigid_dynamic->getGlobalPose();
	physx::PxTransform trns2 = my_shape->getLocalPose();
	float3 capsule_start;
	float3 capsule_vec;
	mat4x4 mat(trns * trns2);
	capsule_start = mat.getPosition() - mat.getBasis(0) * 1.0f;
	capsule_vec = mat.getBasis(0) * 2.0f;
	DrawCapsule3D(capsule_start, capsule_start + capsule_vec, 0.5f, 8, color, color, false);


}

void SampleObject::Exit()
{
	SceneManager::GetCurrentScene()->DeleteActor(rigid_dynamic);
	rigid_dynamic = nullptr;
	//MV1DetachAnim(model, 0);
	//if (anim > 0)
	//	MV1DeleteModel(anim);
	//if (model > 0)
	//	MV1DeleteModel(model);
}

void SampleObject::OnCollisionEnter(const HitInfo& hit_info)
{
	color = CYAN;
	SceneManager::Object::Destory(hit_info.hit_collision->owner);
}

void SampleObject::OnTriggerEnter(const HitInfo& hit_info)
{
	color = YELLOW;
}



