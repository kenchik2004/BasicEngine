#include "precompile.h"
#include "System/Components/RigidBody.h"
#include "MeshCollider.h"
#include "System/Components/ModelRenderer.h"


using namespace physx;

int MeshCollider::Init()
{
	rigidbody = owner.lock()->GetComponent<RigidBody>();
	if (!rigidbody.lock()) {
		RemoveThisComponent();
		return -1;
	}
	rigidbody.lock()->ChangeToStatic();
	model = owner.lock()->GetComponent<ModelRenderer>();
	if (model.lock() && rigidbody.lock())
		AttachToModel();
	return 0;
}

void MeshCollider::PrePhysics()
{
	rigidbody.lock()->GetBody()->detachShape(*shape);

	PxTransform trns = MakeCollisionTransform();
	mesh.scale = owner.lock()->transform->scale;
	shape->setGeometry(mesh);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !is_trigger);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, is_trigger);
	shape->setLocalPose(trns);

	rigidbody.lock()->GetBody()->attachShape(*shape);
}

void MeshCollider::DebugDraw()
{
	auto trns = rigidbody.lock()->GetBody()->getGlobalPose();
	for (int i = 0; i < ref_poly_.PolygonNum; i++) {
		Vector3 p0 = cast(ref_poly_.Vertexs[ref_poly_.Polygons[i].VIndex[0]].Position) * owner.lock()->transform->scale.x;
		Vector3 p1 = cast(ref_poly_.Vertexs[ref_poly_.Polygons[i].VIndex[1]].Position) * owner.lock()->transform->scale.x;
		Vector3 p2 = cast(ref_poly_.Vertexs[ref_poly_.Polygons[i].VIndex[2]].Position) * owner.lock()->transform->scale.x;
		p0 = trns.q.rotate(p0);
		p1 = trns.q.rotate(p1);
		p2 = trns.q.rotate(p2);
		p0 += trns.p;
		p1 += trns.p;
		p2 += trns.p;
		// ポリゴンを形成する三頂点を使用してワイヤーフレームを描画する
		DrawLine3D(cast(p0), cast(p1), RED);

		DrawLine3D(cast(p1), cast(p2), RED);

		DrawLine3D(cast(p2), cast(p0), RED);
	}
}

void MeshCollider::AttachToModel()
{
	if (!model.lock())
		model = owner.lock()->GetComponent<ModelRenderer>();
	if (attached || !model.lock() || !rigidbody.lock())
		return;

	try {
		PxCookingParams params(PhysicsManager::GetPhysicsInstance()->getTolerancesScale());
		params.midphaseDesc = PxMeshMidPhase::eBVH34;
		int handle = model.lock()->model.handle;
		MV1SetupReferenceMesh(handle, -1, false);
		ref_poly_ = MV1GetReferenceMesh(handle, -1, false);
		PxTriangleMeshDesc desc;
		int meshNum = MV1GetMeshNum(handle);
		// 頂点データ取得
		std::vector<PxVec3> vertices;
		for (int i = 0; i < ref_poly_.VertexNum; i++) {
			VECTOR vertex = ref_poly_.Vertexs[i].Position;
			vertices.push_back(Vector3(vertex.x, vertex.y, vertex.z));

		}

		desc.points.count = static_cast<PxU32>(vertices.size());
		desc.points.stride = sizeof(Vector3);
		desc.points.data = vertices.data();

		// インデックスデータ取得
		std::vector<PxU32> indices;
		for (int i = 0; i < ref_poly_.PolygonNum; i++) {
			PxU32 idx0 = ref_poly_.Polygons[i].VIndex[0];
			PxU32 idx1 = ref_poly_.Polygons[i].VIndex[1];
			PxU32 idx2 = ref_poly_.Polygons[i].VIndex[2];

			indices.push_back(idx0);
			indices.push_back(idx1);
			indices.push_back(idx2);
		}

		desc.triangles.count = ref_poly_.PolygonNum;
		desc.triangles.stride = sizeof(PxU32) * 3;
		desc.triangles.data = indices.data();
		desc.flags = PxMeshFlags();

#ifndef NDEBUG
		if (!desc.isValid())
			throw(Exception("ERROR!!DESC_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif

		PxDefaultMemoryOutputStream write_buffer;
		bool status{ PxCookTriangleMesh(params,desc, write_buffer) };
#ifndef NDEBUG
		if (!status)
			throw(Exception("ERROR!!STATUS_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif

		PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
		PxTriangleMesh* triangle_mesh{ PhysicsManager::GetPhysicsInstance()->createTriangleMesh(read_buffer) };

#ifndef NDEBUG
		if (!triangle_mesh)
			throw(Exception("ERROR!!MESH_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif


		mesh.triangleMesh = triangle_mesh;

		shape = { PhysicsManager::GetPhysicsInstance()->createShape(mesh, *Material::Concrete_Default) };
		shape->userData = new SafeWeakPtr<Collider>(std::static_pointer_cast<Collider>(shared_from_this()));
		rigidbody.lock()->GetBody()->attachShape(*shape);
	}
	catch (Exception& ex) {
		ex.Show();
		return;
	}
	attached = true;
}
