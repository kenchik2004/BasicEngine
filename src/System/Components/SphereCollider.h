#pragma once
#include "Collider.h"

USING_PTR(SphereCollider);
class SphereCollider :
    public Collider
{
public:
    USING_SUPER(SphereCollider);
    SphereCollider(Vector3 pos = { 0,0,0 }, Quaternion rot = { 0,0,0,1 }, float radius_ = 0.5f, bool is_trigger_ = false, Layer layer_ = Layer::Default, u8 hit_group_ = Layer::All)
		:Collider(pos, rot, is_trigger_, layer_, hit_group_), radius(radius_) {
	}
    int Init() override;
    void PrePhysics() override;
    void DebugDraw() override;

    const float& Radius() { return radius; }
    float radius = 0.5f;
private:


};

