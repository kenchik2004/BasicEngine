#pragma once
#include <System/Component.h>
class RigidBody :public Component
{
public:
	USING_SUPER(RigidBody);
	int Init() override;
	void PrePhysics() override;
	void PostPhysics() override;
	void Update() override;
	void DebugDraw() override;
	void Exit() override;
	Vector3 velocity = { 0,0,0 };
	bool is_a = false;
	physx::PxRigidActor* GetBody() { return body; }
private:
	physx::PxRigidActor* body = nullptr;
	Vector3 pos = { 0,0,0 };
	Quaternion rot = Quaternion(physx::PxIdentity);
};

