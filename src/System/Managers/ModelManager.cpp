#include "ModelManager.h"

using namespace physx;

std::vector<SafeUniquePtr<ModelSource>>	ModelManager::model_chache;
std::vector<SafeUniquePtr<AnimSource>>	ModelManager::anim_chache;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::m_name;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::m_path;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::a_name;
std::unordered_map<std::string, ModelManager::IndexAndHandle>	ModelManager::a_path;

static SafeSharedPtr<Texture> ibl_diffuse;
static SafeSharedPtr<Texture> ibl_specular;


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
		int cache_index = ptr->model_cache->size();
		auto name = ptr->m_source->name;
		auto path = ptr->m_source->path;
		ptr->model_cache->push_back(std::move(ptr->m_source));
		(*(ptr->m_name))[name].index = cache_index;
		(*(ptr->m_path))[path].index = cache_index;
		loading_count--;
		delete ptr;
		};
	PtrToCacheAndModelData* data = new PtrToCacheAndModelData;
	data->model_cache = &model_chache;
	data->m_name = &m_name;
	data->m_path = &m_path;
	data->m_source = std::move(model);
	SetUseASyncLoadFlag(true);
	data->m_source->handle = MV1LoadModel(path_key.c_str());
	if (data->m_source->handle < 0) {
		delete data;
		SetUseASyncLoadFlag(false);
		return;
	}
	m_name[name_key].handle = data->m_source->handle;
	m_path[path_key].handle = data->m_source->handle;
	SetASyncLoadFinishCallback(data->m_source->handle, call_back, data);
	loading_count++;
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
		int cache_index = ptr->anim_cache->size();

		auto name = ptr->a_source->name;
		auto path = ptr->a_source->path;
		ptr->anim_cache->push_back(std::move(ptr->a_source));
		(*(ptr->a_name))[name].index = cache_index;
		(*(ptr->a_path))[path].index = cache_index;
		loading_count--;
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
	loading_count++;
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
		while (resource->second.index < 0) {}
		if (!model_chache[resource->second.index]->is_initialized)
			model_chache[resource->second.index]->Init();
		model->handle = MV1DuplicateModel(model_chache[resource->second.index]->handle);
		model->name = new_name == "" ? model_chache[resource->second.index]->name : new_name;
		model->original = model_chache[resource->second.index].raw_unique().get();
		for (int i = 0; i < model->original->materials.size(); i++)
			model->materials.push_back(model->original->materials[i]);

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
		while (resource->second.index < 0) {}
		if (!model_chache[resource->second.index]->is_initialized)
			model_chache[resource->second.index]->Init();
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
		while (resource->second.index < 0) {}
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
	MaterialManager::Init();
	SetGraphBlendScalingFilterMode(true);
	//--------------------------
	ibl_diffuse = make_safe_shared<Texture>(LoadGraph("data/IBL/iblDiffuseHDR.dds"));
	ibl_specular = make_safe_shared<Texture>(LoadGraph("data/IBL/iblSpecularHDR.dds"));
	//--------------------------
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
	//--------------------------
	ibl_diffuse.reset();
	ibl_specular.reset();
	//--------------------------
	MaterialManager::Release();
#ifndef PACKAGE_BUILD
	if (Model::instance > 0) {
		std::string msg = typeid(Model).name();
		throw(MemoryLeakException(msg.c_str(), DEFAULT_EXCEPTION_PARAM));
	}
	if (Animation::instance > 0) {
		std::string msg = typeid(Animation).name();
		throw(MemoryLeakException(msg.c_str(), DEFAULT_EXCEPTION_PARAM));
	}
#endif

}
