#pragma once
#include "Collider.h"

USING_PTR(CapsuleCollider);
class CapsuleCollider :
	public Collider
{
public:
	USING_SUPER(CapsuleCollider);
	CapsuleCollider(float height_ = 1.0f, float radius_ = 0.5f,
		Vector3 pos = { 0,0,0 },Quaternion rot = { 0,0,0,1 },
		bool is_trigger_ = false,
		Collider::Layer layer_ = Collider::Layer::Default,
		u32 hit_group_ = Collider::Layer::All)
		:Collider(pos, rot, is_trigger_, layer_, hit_group_),
		height(height_), radius(radius_) {
	}
	int Init()override;
	void PrePhysics()override;
	void DebugDraw() override;
	float height = 1.0f;
	float radius = 0.5f;
};

