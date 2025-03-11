#pragma once
#include "System/Components/Collider.h"


USING_PTR(ModelRenderer);
class MeshCollider :
	public Collider
{
public:
	int Init() override;
	void PrePhysics() override;
	void DebugDraw() override;
	void AttachToModel();

private:
	ModelRendererWP model;
	physx::PxTriangleMeshGeometry mesh{};
	bool attached = false;
	MV1_REF_POLYGONLIST                   ref_poly_{};       //!< ポリゴンデータ
};

