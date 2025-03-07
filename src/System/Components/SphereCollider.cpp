#include "precompile.h"
#include "RigidBody.h"
#include "SphereCollider.h"
#include <System/Components/ModelRenderer.h>

using namespace physx;
int SphereCollider::Init()
{
	rigidbody = owner->GetComponent<RigidBody>();
	if (!rigidbody.lock()) {
		RemoveThisComponent();
		return -1;
	}
	auto p_scene = SceneManager::GetCurrentScene()->GetPhysicsScene();

	shape = PhysicsManager::GetPhysicsInstance()->createShape(
		PxSphereGeometry(radius),
		*Material::Metal_Default);
	shape->userData = new std::weak_ptr<Collider>(std::static_pointer_cast<Collider>(shared_from_this()));
	shape->setSimulationFilterData(PxFilterData(hit_group, collision_group, 0, 0));


	rigidbody.lock()->GetBody()->attachShape(*shape);
	return 0;
}

void SphereCollider::PrePhysics()
{
	rigidbody.lock()->GetBody()->detachShape(*shape);

	PxTransform trns = MakeCollisionTransform();
	shape->setGeometry(PxSphereGeometry(radius));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !is_trigger);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, is_trigger);
	shape->setLocalPose(trns);



	rigidbody.lock()->GetBody()->attachShape(*shape);
}

void SphereCollider::DebugDraw()
{
	auto body_trns = rigidbody.lock()->GetBody()->getGlobalPose();
	auto shape_trns = shape->getLocalPose();
	mat4x4 mat(body_trns * shape_trns);
	DrawSphere3D(cast(mat.getPosition()), radius, 8, GREEN, GREEN, false);
}


