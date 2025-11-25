#include "RigidBody.h"
#include "SphereCollider.h"
#include <System/Components/ModelRenderer.h>

using namespace physx;
int SphereCollider::Init()
{
	rigidbody = owner->GetComponent<RigidBody>();
	if (!rigidbody) {
		RemoveThisComponent();
		return -1;
	}
	auto p_scene = owner->GetScene()->GetPhysicsScene();

	shape = PhysicsManager::GetPhysicsInstance()->createShape(
		PxSphereGeometry(radius),
		*PhysicMaterial::Default, true);
	shape->userData = new SafeWeakPtr<Collider>(std::static_pointer_cast<Collider>(shared_from_this()));
	shape->setSimulationFilterData(PxFilterData(collision_group, hit_group, 0, 0));
	shape->setQueryFilterData(PxFilterData(collision_group, hit_group, 0, 0));
	PxTransform trns = MakeCollisionTransform();
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !is_trigger);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, is_trigger);
	shape->setLocalPose(trns);


	rigidbody->GetBody()->attachShape(*shape);
	return 0;
}

void SphereCollider::PrePhysics()
{

	PxTransform trns = MakeCollisionTransform();
	shape->setGeometry(PxSphereGeometry(radius));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !is_trigger);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, is_trigger);
	shape->setLocalPose(trns);

}

void SphereCollider::DebugDraw()
{
	auto body_trns = rigidbody->GetBody()->getGlobalPose();
	auto shape_trns = shape->getLocalPose();
	mat4x4 mat(body_trns * shape_trns);
	DrawSphere3D(cast(mat.getPosition()), radius, 8, Color::GREEN, Color::GREEN, false);
}


