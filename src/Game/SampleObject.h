#pragma once
USING_PTR(SampleObject);
class SampleObject :public Object
{
public:
	USING_SUPER(SampleObject);
	int Init() override;
	void Update() override;
	void Draw() override;
	void PreDraw() override;
	void PrePhysics() override;
	void PostPhysics() override;
	void DebugDraw()override;
	void Exit()override;
	physx::PxRigidDynamic* rigid_dynamic = nullptr;
	physx::PxShape* my_shape = nullptr;
	Vector3 velocity;
	bool camera = false;
	int model = -1;
};
class SampleObject2 :public Object
{
public:
	USING_SUPER(SampleObject2);
	int Init() override;
	void Draw() override;
	void Update() override;
	void PrePhysics() override;
	void PostPhysics() override;
	void OnCollisionEnter(const HitInfo& hit_info) override;
	void OnCollisionStay(const HitInfo& hit_info) override;
	void OnCollisionExit(const HitInfo& hit_info) override;
	void DebugDraw()override;
	void Exit()override;
	physx::PxRigidDynamic* rigid_dynamic = nullptr;
	physx::PxShape* my_shape = nullptr;
	unsigned int color;
	Vector3 velocity;
};
class SampleObject3 :public Object
{
public:
	USING_SUPER(SampleObject3);
	int Init() override;
	void Draw() override;
	void PrePhysics() override;
	void PostPhysics() override;
	void OnCollisionEnter(const HitInfo& hit_info) override;
	void OnCollisionStay(const HitInfo& hit_info) override;
	void OnCollisionExit(const HitInfo& hit_info) override;
	void DebugDraw()override;
	void Exit()override;
	ObjBaseP obj = nullptr;
	physx::PxRigidDynamic* rigid_dynamic = nullptr;
	physx::PxShape* my_shape = nullptr;
	unsigned int color;
	Vector3 velocity;
};


