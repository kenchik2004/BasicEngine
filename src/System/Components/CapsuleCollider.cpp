#include "precompile.h"
#include <System/Components/RigidBody.h>
#include "System/Components/CapsuleCollider.h"
#include "System/Components/ModelRenderer.h"

using namespace physx;
int CapsuleCollider::Init()
{
	rigidbody = owner.lock()->GetComponent<RigidBody>();
	if (!rigidbody.lock()) {
		RemoveThisComponent();
		return -1;
	}
	auto p_scene = owner.lock()->GetScene()->GetPhysicsScene();

	shape = PhysicsManager::GetPhysicsInstance()->createShape(
		PxCapsuleGeometry(radius, height * 0.5f),
		*Material::Metal_Default);
	shape->userData = new SafeWeakPtr<Collider>(std::static_pointer_cast<Collider>(shared_from_this()));
	shape->setSimulationFilterData(PxFilterData(hit_group, collision_group, 0, 0));
	rigidbody.lock()->GetBody()->attachShape(*shape);
	return 0;
}

void CapsuleCollider::PrePhysics()
{
	auto body = rigidbody.lock()->GetBody();
	body->detachShape(*shape);

	PxTransform trns = MakeCollisionTransform();
	shape->setGeometry(PxCapsuleGeometry(radius, height * 0.5f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !is_trigger);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, is_trigger);
	shape->setLocalPose(trns);

	body->attachShape(*shape);
}

void CapsuleCollider::DebugDraw()
{
	auto body = rigidbody.lock()->GetBody();
	physx::PxTransform trns = body->getGlobalPose();
	physx::PxTransform trns2 = shape->getLocalPose();
	float3 capsule_start;
	float3 capsule_vec;
	mat4x4 mat(trns * trns2);
	capsule_start = mat.getPosition() - mat.getBasis(0) * height * 0.5f;
	capsule_vec = mat.getBasis(0) * height;
	DrawCapsule3D(capsule_start, capsule_start + capsule_vec, radius, 8, Color::GREEN, Color::GREEN, false);
}
