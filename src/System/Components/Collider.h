#pragma once
#include "System/Component.h"

USING_PTR(RigidBody);
class Collider :
	public Component
{
public:
	USING_SUPER(Collider);
	bool is_trigger = false;
	int Init() override;
	void Update() override;
	void Exit() override;
	Vector3 position = { 0,0,0 };
	Quaternion rotation = { 0,0,0,1 };
	RigidBodyP GetRigidBody() { return rigidbody.lock(); }
	void AttachToModel(int attach_index);


	physx::PxU32 hit_group = UINT32_MAX;
	physx::PxU32 collision_group = 1;
protected:
	bool attach_to_model = false;
	int model_attach_index = -1;
	RigidBodyWP rigidbody;
	physx::PxShape* shape;

	physx::PxTransform MakeCollisionTransform();

};

