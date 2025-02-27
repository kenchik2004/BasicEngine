#include "precompile.h"
#include "SampleObject.h"
#include "System/Components/RigidBody.h"
#include <System/Components/Collider.h>

mat4x4 MV1GetFrameLocalWorldMatrix(int MHandle, int FrameIndex, bool is_physx) {
	mat4x4 mat = cast(MV1GetFrameLocalWorldMatrix(MHandle, FrameIndex));
	mat = CastPhysXMat(mat);
	return mat;
}


int SampleObject::Init()
{
	transform->SetPosition(float3(0, 0, 5));

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


}

void SampleObject::PreDraw()
{
}
void SampleObject::PrePhysics()
{


	rigid_dynamic->setGlobalPose(physx::PxTransform(transform->position, transform->rotation));

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



