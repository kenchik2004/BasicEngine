#pragma once
#include <System/Component.h>
struct LockFlag {
	bool x = false;
	bool y = false;
	bool z = false;
};
enum ForceMode  {		//�͂������郂�[�h
	Force = physx::PxForceMode::eFORCE,						//�d�������������P���I�ȗ͂̕ω�(N/s^2)
	Impulse = physx::PxForceMode::eIMPULSE,					//�d�������������p���I�ȉ^���ʂ̕ω�(N/s)
	VelocityCange = physx::PxForceMode::eVELOCITY_CHANGE,	//�d���𖳎��������x�̕ω�(m/s)
	Acceleration = physx::PxForceMode::eACCELERATION,		//�d���𖳎����������x�̕ω�(m/s^2)
};
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
	void AddForce(Vector3 force, ForceMode force_mode = ForceMode::Force);
	void SetMassCenter(Vector3 center);
	void SetVelocity(Vector3 velocity_);
	Vector3 velocity = { 0,0,0 };
	float mass = 1.0f;
	physx::PxRigidActor* GetBody() { return body; }
	LockFlag freeze_position = { 0,0,0 };
	LockFlag freeze_rotation = { 0,0,0 };
	bool use_gravity = true;
	bool is_kinematic = false;
	void ChangeToStatic();

private:
	physx::PxRigidActor* body = nullptr;
	Vector3 pos = { 0,0,0 };
	Quaternion rot = Quaternion(physx::PxIdentity);
};

