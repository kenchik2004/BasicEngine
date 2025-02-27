#include "precompile.h"
#include <System/Components/RigidBody.h>
#include "BoxCollider.h"
#include <System/Components/ModelRenderer.h>

using namespace physx;

int BoxCollider::Init()
{
	rigidbody = owner->GetComponent<RigidBody>();
	if (!rigidbody.lock()) {
		RemoveThisComponent();
		return -1;
	}
	auto p_scene = SceneManager::GetCurrentScene()->GetPhysicsScene();

	shape = PhysicsManager::GetPhysicsInstance()->createShape(
		PxBoxGeometry(extension.x * 0.5f, extension.y * 0.5f, extension.z * 0.5f),
		*Material::Metal_Default);
	shape->userData = new std::weak_ptr<Collider>(std::static_pointer_cast<Collider>(shared_from_this()));

	rigidbody.lock()->GetBody()->attachShape(*shape);
	return 0;
}

void BoxCollider::PrePhysics()
{
	rigidbody.lock()->GetBody()->detachShape(*shape);


	PxTransform trns = MakeCollisionTransform();

	shape->setGeometry(PxBoxGeometry(extension.x * 0.5f, extension.y * 0.5f, extension.z * 0.5f));
	shape->setLocalPose(trns);



	rigidbody.lock()->GetBody()->attachShape(*shape);
}

void BoxCollider::DebugDraw()
{
	auto body = rigidbody.lock()->GetBody();
	physx::PxTransform trns = body->getGlobalPose();
	physx::PxTransform trns2 = shape->getLocalPose();
	mat4x4 mat(trns * trns2);
	Vector3 points[8] = {
		{extension.x * 0.5f,extension.y * 0.5f,extension.z * 0.5f},
		{extension.x * 0.5f,extension.y * 0.5f,extension.z * -0.5f},
		{extension.x * 0.5f,extension.y * -0.5f,extension.z * -0.5f},
		{extension.x * 0.5f,extension.y * -0.5f,extension.z * 0.5f},
		{extension.x * -0.5f,extension.y * 0.5f,extension.z * 0.5f},
		{extension.x * -0.5f,extension.y * 0.5f,extension.z * -0.5f},
		{extension.x * -0.5f,extension.y * -0.5f,extension.z * -0.5f},
		{extension.x * -0.5f,extension.y * -0.5f,extension.z * 0.5f},
	};
	for (int i = 0; i < 8; i++) {
		points[i] = mat.getPosition() + mat.rotate(points[i]);
	}
	for (int i = 0; i < 4; i++) {
		DrawLine3D(cast(points[i]), i != 3 ? cast(points[i + 1]) : cast(points[0]), GREEN);
		DrawLine3D(cast(points[i]), cast(points[i + 4]), GREEN);
		if (i % 2 == 0 && i != 0) {
			DrawLine3D(cast(points[i]), cast(points[i - 2]), GREEN);
		}
	}
	for (int i = 4; i < 8; i++) {
		DrawLine3D(cast(points[i]), i != 7 ? cast(points[i + 1]) : cast(points[4]), GREEN);
		if (i % 2 == 0 && i != 4) {
			DrawLine3D(cast(points[i]), cast(points[i - 2]), GREEN);
		}
	}
}
