#include "Collider.h"
#include <System/Components/RigidBody.h>
#include <System/Components/ModelRenderer.h>

using namespace physx;
void Collider::Construct()
{
	SetPriority(1);
}
void Collider::OnSleep()
{

	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
}
void Collider::OnWakeUp()
{
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !is_trigger);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, is_trigger);
}
int Collider::Init()
{
	rigidbody = owner->GetComponent<RigidBody>();
	if (!rigidbody) {
		RemoveThisComponent();
		return -1;
	}
	auto p_scene = owner->GetScene()->GetPhysicsScene();

	shape = PhysicsManager::GetPhysicsInstance()->createShape(
		PxSphereGeometry(),
		*PhysicMaterial::Default);
	shape->userData = new SafeWeakPtr<Collider>(std::static_pointer_cast<Collider>(shared_from_this()));
	shape->setSimulationFilterData(PxFilterData(collision_group, hit_group, 0, 0));
	shape->setQueryFilterData(PxFilterData(collision_group, hit_group, 0, 0));

	rigidbody->GetBody()->attachShape(*shape);

	return 0;
}


void Collider::Exit()
{
	if (rigidbody) {
		if (auto body = rigidbody->GetBody())
			body->detachShape(*shape);
	}
	owner->GetScene()->DeleteShape(shape);
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

void Collider::SetMaterial(physx::PxMaterial* new_mat)
{
	if (!shape || !new_mat)
		return;
	shape->setMaterials(&new_mat, 1);
}

void Collider::SetLayer(Layer layer)
{
	PxRigidActor* body = rigidbody->GetBody();
	body->detachShape(*shape);
	collision_group = layer;
	shape->setSimulationFilterData(PxFilterData(collision_group, hit_group, 0, 0));
	shape->setQueryFilterData(PxFilterData(collision_group, hit_group, 0, 0));
	body->attachShape(*shape);
}

void Collider::SetHitGroup(u32 hit_layer)
{

	PxRigidActor* body = rigidbody->GetBody();
	body->detachShape(*shape);
	hit_group = hit_layer;
	shape->setSimulationFilterData(PxFilterData(collision_group, hit_group, 0, 0));
	shape->setQueryFilterData(PxFilterData(collision_group, hit_group, 0, 0));
	body->attachShape(*shape);
}

PxTransform Collider::MakeCollisionTransform()
{
	Vector3 pos = { 0,0,0 };
	Quaternion rot = { 0,0,0,1 };
	auto model = owner->GetComponent<ModelRenderer>();
	if (model)
		if (model->IsLoaded())
			if (attach_to_model && model) {
				float3 frame_pos = MV1GetFramePosition(model->GetModelHandle(), model_attach_index);
				mat4x4 frame_mat = MV1GetFrameLocalWorldMatrix(model->GetModelHandle(), model_attach_index, true);
				physx::PxTransform t(frame_mat);
				frame_pos -= cast(MV1GetPosition(model->GetModelHandle()));
				Quaternion q = Inverse(owner->transform->rotation);
				pos = q.rotate(frame_pos);
				rot = q * t.q;
			}
	rot *= rotation;
	pos += rot.rotate(position);
	return physx::PxTransform(pos, rot);

}
