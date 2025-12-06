#pragma once
#include "Collider.h"
USING_PTR(BoxCollider);
class BoxCollider :
	public Collider
{
public:
	USING_SUPER(BoxCollider);
	BoxCollider(Vector3 pos = { 0,0,0 }, Quaternion rot = { 0,0,0,1 }, Vector3 ext = { 1.0f,1.0f,1.0f }, bool is_trigger_ = false,
		Layer layer_ = Layer::Default, u8 hit_group_ = Layer::All)
		:Collider(pos, rot, is_trigger_, layer_, hit_group_), extension(ext) {
	}
	int Init() override;
	void PrePhysics() override;
	void DebugDraw() override;
	Vector3 extension = { 1.0f,1.0f,1.0f };
};

