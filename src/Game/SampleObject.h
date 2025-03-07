#pragma once
USING_PTR(SampleObject);
USING_PTR(CapsuleCollider);
class SampleObject :public Object
{
public:
	USING_SUPER(SampleObject);
	int Init() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;
	void PreDraw() override;
	void PrePhysics() override;
	void PostPhysics() override;
	void DebugDraw()override;
	void Exit()override;

	void OnCollisionEnter(const HitInfo& hit_info) override;
	void OnTriggerEnter(const HitInfo& hit_info) override;
	physx::PxRigidDynamic* rigid_dynamic = nullptr;
	physx::PxShape* my_shape = nullptr;
	Vector3 velocity = { 0,0,0 };
	CapsuleColliderWP fist;

	unsigned int color = MAGENTA;
};