#pragma once

USING_PTR(RigidBody);
USING_PTR(Collider);
class Collider :
	public Component
{
public:
	USING_SUPER(Collider);
	bool is_trigger = false;
	void Construct() override;
	int Init() override;
	void Exit() override;
	Vector3 position = { 0,0,0 };
	Quaternion rotation = { 0,0,0,1 };
	RigidBodyP GetRigidBody() { return rigidbody.lock(); }
	void AttachToModel(int attach_index);
	void SetMaterial(physx::PxMaterial* new_mat);
	enum Layer :physx::PxU32 {
		None = 0,
		Default = 1,
		Terrain = 1 << 1,
		Wepon = 1 << 2,
		Enemy = 1 << 3,
		Player = 1 << 4,
		Vehicle = 1 << 5,

		All = UINT32_MAX,
	};
	void SetLayer(Layer layer);
	Layer GetLayer() { return collision_group; }
	u32 GetHitGroup() { return hit_group; }
	void SetHitGroup(u32 hit_layer);
protected:
	u32 hit_group = All;
	Layer collision_group = Default;
	bool attach_to_model = false;
	int model_attach_index = -1;
	RigidBodyWP rigidbody;
	physx::PxShape* shape;

	physx::PxTransform MakeCollisionTransform();

};

