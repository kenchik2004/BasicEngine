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
		*Material::Metal_Default);

	shape->userData = new std::weak_ptr<Collider>(std::static_pointer_cast<Collider>(shared_from_this()));

	rigidbody.lock()->GetBody()->attachShape(*shape);

	return 0;
}

void Collider::Update()
{
}

void Collider::Exit()
{
	if (rigidbody.lock()) {
		if (auto body = rigidbody.lock()->GetBody())
			body->detachShape(*shape);
	}
	SceneManager::GetCurrentScene()->DeleteShape(shape);
	shape = nullptr;
}
