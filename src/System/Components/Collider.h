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
protected:
	RigidBodyWP rigidbody;
	physx::PxShape* shape;
};

