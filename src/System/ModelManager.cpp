#include "precompile.h"
#include "ModelManager.h"

using namespace physx;

std::vector<SafeUniquePtr<ModelSource>>	ModelManager::model_chache;
std::vector<SafeUniquePtr<AnimSource>>	ModelManager::anim_chache;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::m_name;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::m_path;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::a_name;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::a_path;


void ModelManager::LoadAsModel(std::string_view path, std::string_view name)
{
	std::string path_key(path);
	std::string name_key(name);
	if (m_path.count(path_key) || m_name.count(name_key))
		return;
	auto model = make_safe_unique<ModelSource>();
	model->name = name_key;
	model->path = path_key;
	auto&& call_back = [](int mv1_handle, void* data) {
		auto ptr = reinterpret_cast<PtrToCacheAndModelData*>(data);
		ptr->m_source->is_loaded = true;
		int cache_index = ptr->model_cache->size();
		auto name = ptr->m_source->name;
		auto path = ptr->m_source->path;
		(*(ptr->m_name))[name].index = cache_index;
		(*(ptr->m_path))[path].index = cache_index;
		ptr->model_cache->push_back(std::move(ptr->m_source));
		delete ptr;
		};
	PtrToCacheAndModelData* data = new PtrToCacheAndModelData;
	data->model_cache = &model_chache;
	data->m_name = &m_name;
	data->m_path = &m_path;
	data->m_source = std::move(model);
	SetUseASyncLoadFlag(true);
	data->m_source->handle = MV1LoadModel(path_key.c_str());
	m_name[name_key].handle = data->m_source->handle;
	m_path[path_key].handle = data->m_source->handle;
	SetASyncLoadFinishCallback(data->m_source->handle, call_back, data);
	SetUseASyncLoadFlag(false);
	return;
}

void ModelManager::LoadAsAnimation(std::string_view path, std::string_view name)
{
	std::string path_key(path);
	std::string name_key(name);
	if (a_path.count(path_key) || a_name.count(name_key))
		return;
	auto anim = make_safe_unique<AnimSource>();
	anim->name = name_key;
	anim->path = path_key;
	auto call_back = [](int mv1_handle, void* data) {
		auto ptr = reinterpret_cast<PtrToCacheAndModelData*>(data);
		ptr->a_source->is_loaded = true;
		int cache_index = ptr->anim_cache->size();
		auto name = ptr->a_source->name;
		auto path = ptr->a_source->path;
		(*(ptr->a_name))[name].index = cache_index;
		(*(ptr->a_path))[path].index = cache_index;
		ptr->anim_cache->push_back(std::move(ptr->a_source));
		delete ptr;
		};
	PtrToCacheAndModelData* data = new PtrToCacheAndModelData;
	data->anim_cache = &anim_chache;
	data->a_name = &a_name;
	data->a_path = &a_path;
	data->a_source = std::move(anim);
	SetUseASyncLoadFlag(true);
	data->a_source->handle = MV1LoadModel(path_key.c_str());
	a_name[name_key].handle = data->a_source->handle;
	a_path[path_key].handle = data->a_source->handle;
	SetASyncLoadFinishCallback(data->a_source->handle, call_back, data);
	SetUseASyncLoadFlag(false);
}

SafeSharedPtr<Model> ModelManager::CloneModelByName(std::string_view name, std::string_view new_name)
{
	std::string name_(name);
	SafeSharedPtr<Model> model = nullptr;
	auto resource = m_name.find(name_);
	if (resource != m_name.end() && resource->second.handle >= 0) {
		model = make_safe_shared<Model>();
		if (resource->second.index < 0)
			WaitHandleASyncLoad(resource->second.handle);
		model->handle = MV1DuplicateModel(model_chache[resource->second.index]->handle);
		model->name = new_name == "" ? model_chache[resource->second.index]->name : new_name;
		model->original = model_chache[resource->second.index].raw_unique().get();
	}

	return model;
}

SafeSharedPtr<Model> ModelManager::CloneModelByPath(std::string_view path, std::string_view new_name)
{
	std::string path_(path);
	SafeSharedPtr<Model> model = nullptr;
	auto resource = m_path.find(path_);
	if (resource != m_path.end() && resource->second.handle >= 0) {
		model = make_safe_shared<Model>();
		if (resource->second.index < 0)
			WaitHandleASyncLoad(resource->second.handle);
		model->handle = MV1DuplicateModel(model_chache[resource->second.index]->handle);
		model->name = new_name == "" ? model_chache[resource->second.index]->name : new_name;
		model->original = model_chache[resource->second.index].raw_unique().get();
	}

	return model;
}

SafeSharedPtr<Animation> ModelManager::CloneAnimByName(std::string_view name, int index, std::string_view new_name)
{
	std::string name_(name);
	SafeSharedPtr<Animation> anim = nullptr;
	auto resource = a_name.find(name_);
	if (resource != a_name.end() && resource->second.handle >= 0) {
		anim = make_safe_shared<Animation>();
		if (resource->second.index < 0)
			WaitHandleASyncLoad(resource->second.handle);
		anim->handle = MV1DuplicateModel(anim_chache[resource->second.index]->handle);
		anim->total_time = MV1GetAnimTotalTime(anim->handle, index);
		anim->name = new_name == "" ? anim_chache[resource->second.index]->name : new_name;
		anim->index = index;
	}

	return anim;
}

SafeSharedPtr<Animation> ModelManager::CloneAnimByPath(std::string_view path, int index, std::string_view new_name)
{
	std::string path_(path);
	SafeSharedPtr<Animation> anim = nullptr;
	auto resource = a_path.find(path_);
	if (resource != a_path.end() && resource->second.handle >= 0) {
		anim = make_safe_shared<Animation>();
		if (resource->second.index < 0)
			WaitHandleASyncLoad(resource->second.handle);
		anim->handle = MV1DuplicateModel(anim_chache[resource->second.index]->handle);
		anim->total_time = MV1GetAnimTotalTime(anim->handle, anim->index);
		anim->name = new_name == "" ? anim_chache[resource->second.index]->name : new_name;
		anim->index = index;
	}

	return anim;
}

void ModelManager::Init()
{
}

void ModelManager::Exit()
{
	WaitHandleASyncLoadAll();
	model_chache.clear();
	anim_chache.clear();
	m_name.clear();
	m_path.clear();
	a_name.clear();
	a_path.clear();
	if (Model::instance > 0) {
		std::string msg = typeid(Model).name();
		throw(MemoryLeakException(msg.c_str(), DEFAULT_EXCEPTION_PARAM));
	}
	if (Animation::instance > 0) {
		std::string msg = typeid(Animation).name();
		throw(MemoryLeakException(msg.c_str(), DEFAULT_EXCEPTION_PARAM));
	}

}

MV1_REF_POLYGONLIST* ModelSource::GetPolygon()
{
	if (!polygons_loaded) {
		MV1SetupReferenceMesh(handle, -1, false);
		ref_poly_ = MV1GetReferenceMesh(handle, -1, false);
	} return &ref_poly_;
}

physx::PxConvexMesh* ModelSource::GetOrCreateConvexMesh()
{
	if (!convex_mesh) {
		PxCookingParams params(PhysicsManager::GetPhysicsInstance()->getTolerancesScale());
		if (handle < 0)
			throw(Exception("凸メッシュ作成に失敗しました:モデルデータが無効です。モデルパスが有効か確認してください。", DEFAULT_EXCEPTION_PARAM));
		GetPolygon();
		PxConvexMeshDesc  desc;
		// 頂点データ取得
		std::vector<PxVec3> vertices;
#if 1
		for (int i = 0; i < ref_poly_.VertexNum; i++) {

			VECTOR vertex = ref_poly_.Vertexs[i].Position;
			Vector3 vert = Vector3(vertex.x, -vertex.z, vertex.y);
			vertices.push_back(vert);
			ref_poly_.Vertexs[i].Position = VGet(vert.x, vert.y, vert.z);

		}
#if 0
		{

			std::ofstream ostream("data/dice/vertex.txt");
			if (!ostream.fail())
			{

				ostream.clear();
				ostream << "[vertex]\n\n";
				for (int i = 0; i < vertices.size(); i++) {
					std::string str;
					str += "{";
					str += std::to_string(vertices[i].x) + "," + std::to_string(vertices[i].y) + "," + std::to_string(vertices[i].z) + "},\n";

					ostream << str.c_str();

				}
				ostream << "\n";
				ostream.close();
			}
		}
#endif


#endif



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
#if 0
		{
			std::ofstream ostream("data/dice/vertex.txt", std::ios::app);
			if (!ostream.fail())
			{

				ostream << "[index]\n\n";
				for (int i = 0; i < indices.size() - 2; i += 3) {
					std::string str;
					str += "{";
					str += std::to_string(indices[i]) + ",";
					str += std::to_string(indices[i + 1]) + ",";
					str += std::to_string(indices[i + 2]) + "},\n";

					ostream << str.c_str();

				}
				ostream.close();
			}
		}
#endif





		//凸メッシュ作成

		desc.points.count = static_cast<PxU32>(vertices.size());
		desc.points.stride = sizeof(Vector3);
		desc.points.data = vertices.data();
		desc.indices.count = ref_poly_.PolygonNum;
		desc.indices.stride = sizeof(PxU32) * 3;
		desc.indices.data = indices.data();
		desc.flags |= PxConvexFlag::eCOMPUTE_CONVEX;
		desc.flags |= PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES;
		desc.flags |= PxConvexFlag::eFAST_INERTIA_COMPUTATION;

#ifndef NDEBUG
		if (!desc.isValid())
			throw(Exception("ERROR!!DESC_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif
		// CookingしてConvexMeshを作成
		PxDefaultMemoryOutputStream write_buffer;
		bool status{ PxCookConvexMesh(params, desc, write_buffer) };

#ifndef NDEBUG
		if (!status)
			throw(Exception("ERROR!!STATUS_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif

		PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
		convex_mesh = PhysicsManager::GetPhysicsInstance()->createConvexMesh(read_buffer);

		if (!convex_mesh)
			throw(Exception("ERROR!!MESH_INVALID", DEFAULT_EXCEPTION_PARAM));

	}

	return convex_mesh;
}

physx::PxTriangleMesh* ModelSource::GetOrCreateTriangleMesh()
{
	if (!triangle_mesh) {
		physx::PxCookingParams params(PhysicsManager::GetPhysicsInstance()->getTolerancesScale());
		params.midphaseDesc = physx::PxMeshMidPhase::eBVH34;
		GetPolygon();
		physx::PxTriangleMeshDesc desc;
		int meshNum = MV1GetMeshNum(handle);
		// 頂点データ取得
		std::vector<physx::PxVec3> vertices;
		for (int i = 0; i < ref_poly_.VertexNum; i++) {
			VECTOR vertex = ref_poly_.Vertexs[i].Position;
			vertices.push_back(Vector3(vertex.x, vertex.y, vertex.z));

		}

		desc.points.count = static_cast<physx::PxU32>(vertices.size());
		desc.points.stride = sizeof(Vector3);
		desc.points.data = vertices.data();





		// インデックスデータ取得
		std::vector<physx::PxU32> indices;
		for (int i = 0; i < ref_poly_.PolygonNum; i++) {
			physx::PxU32 idx0 = ref_poly_.Polygons[i].VIndex[0];
			physx::PxU32 idx1 = ref_poly_.Polygons[i].VIndex[1];
			physx::PxU32 idx2 = ref_poly_.Polygons[i].VIndex[2];

			indices.push_back(idx0);
			indices.push_back(idx1);
			indices.push_back(idx2);
		}

		desc.triangles.count = ref_poly_.PolygonNum;
		desc.triangles.stride = sizeof(physx::PxU32) * 3;
		desc.triangles.data = indices.data();
		desc.flags = physx::PxMeshFlags();

#ifndef NDEBUG
		if (!desc.isValid())
			throw(Exception("ERROR!!DESC_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif

		physx::PxDefaultMemoryOutputStream write_buffer;
		bool status{ PxCookTriangleMesh(params,desc, write_buffer) };
#ifndef NDEBUG
		if (!status)
			throw(Exception("ERROR!!STATUS_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif

		physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
		triangle_mesh = PhysicsManager::GetPhysicsInstance()->createTriangleMesh(read_buffer);

#ifndef NDEBUG
		if (!triangle_mesh)
			throw(Exception("ERROR!!MESH_INVALID", DEFAULT_EXCEPTION_PARAM));
#endif



	}
	return triangle_mesh;
}

physx::PxConvexMesh* Model::GetConvexMesh()
{
	return original->GetOrCreateConvexMesh();
}

physx::PxTriangleMesh* Model::GetTriangleMesh()
{
	return original->GetOrCreateTriangleMesh();
}
