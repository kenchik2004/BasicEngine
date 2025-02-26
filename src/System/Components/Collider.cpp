#include "precompile.h"
#include "Collider.h"
#include <System/Components/RigidBody.h>

using namespace physx;
int Collider::Init()
{
	rigidbody = owner->GetComponent<RigidBody>();
	if (!rigidbody.lock()) {
		RemoveThisComponent();
		return -1;
	}
	auto p_scene = SceneManager::GetCurrentScene()->GetPhysicsScene();

	shape = PhysicsManager::GetPhysicsInstance()->createShape(
		PxSphereGeometry(),
		*PhysicsManager::GetPhysicsInstance()->createMaterial(0.99f, 0.99f, 0.1f));

	return 0;
}

void Collider::Update()
{
}

void Collider::Exit()
{
	if (rigidbody.lock()) {
		rigidbody.lock()->GetBody()->detachShape(*shape);
	}
	SceneManager::GetCurrentScene()->DeleteShape(shape);
	shape = nullptr;
}
