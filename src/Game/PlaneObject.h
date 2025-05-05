#pragma once
#include "ObjBase.h"

USING_PTR(ModelRenderer);
USING_PTR(RigidBody);
USING_PTR(CapsuleCollider);
class PlaneObject :
	public Object
{
public:
	USING_SUPER(PlaneObject);
	int Init()override;
	void Update() override;
	void PreDraw() override;
	void LateDraw() override;
	void OnCollisionEnter(const HitInfo& hit_info) override;

private:
	RigidBodyWP rb;
	CapsuleColliderWP col;
	ModelRendererWP model;
	float speed = 0;
	const float SPEED_MAX = 5.0f;
	Quaternion camera_rot = { 0,0,0,1 };
};

