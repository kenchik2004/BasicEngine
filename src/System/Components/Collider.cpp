#include "precompile.h"
#include "Collider.h"
#include <System/Components/RigidBody.h>
#include <System/Components/ModelRenderer.h>

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

void Collider::AttachToModel(int attach_index)
{
	auto model = owner->GetComponent<ModelRenderer>();
	if (!model)
		return;
	attach_to_model = true;
	model_attach_index = attach_index;
}

PxTransform Collider::MakeCollisionTransform()
{
	Vector3 pos = { 0,0,0 };
	Quaternion rot = { 0,0,0,1 };
	auto model = owner->GetComponent<ModelRenderer>();
	if (attach_to_model && model) {
		float3 frame_pos = MV1GetFramePosition(model->model.handle, model_attach_index);
		mat4x4 frame_mat = MV1GetFrameLocalWorldMatrix(model->model.handle, model_attach_index, true);
		physx::PxTransform t(frame_mat);
		frame_pos -= owner->transform->position;
		Quaternion q = Inverse(owner->transform->rotation);
		pos = q.rotate(frame_pos);
		rot = q * t.q;
	}
	rot *= rotation;
	pos += rot.rotate(position);
	return physx::PxTransform(pos, rot);

}
