#pragma once


class ModelManager
{
	ModelManager() = delete;
	ModelManager(const ModelManager&) = delete;
	~ModelManager() = default;
public:
	static std::vector<SafeUniquePtr<ModelSource>> model_chache;
	static std::vector<SafeUniquePtr<AnimSource>> anim_chache;
	struct IndexAndHandle {
		int index = -1;
		int handle = -1;
	};
	static inline std::atomic<int> loading_count = 0;
	struct PtrToCacheAndModelData {
		std::vector<SafeUniquePtr<ModelSource>>* model_cache = nullptr;
		std::vector<SafeUniquePtr<AnimSource>>* anim_cache = nullptr;
		SafeUniquePtr<ModelSource> m_source;
		SafeUniquePtr<AnimSource> a_source;
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* m_name = nullptr;
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* m_path = nullptr;
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* a_name = nullptr;
		std::unordered_map<std::string, ModelManager::IndexAndHandle>* a_path = nullptr;
	};
	static std::unordered_map<std::string, IndexAndHandle> m_name;
	static std::unordered_map<std::string, IndexAndHandle> m_path;
	static std::unordered_map<std::string, IndexAndHandle> a_name;
	static std::unordered_map<std::string, IndexAndHandle> a_path;
	static void LoadAsModel(std::string_view path, std::string_view name);
	static void LoadAsAnimation(std::string_view path, std::string_view name);
	static SafeSharedPtr<Model> CloneModelByName(std::string_view name, std::string_view new_name = "");
	static SafeSharedPtr<Model> CloneModelByPath(std::string_view path, std::string_view new_name = "");
	static SafeSharedPtr<Animation> CloneAnimByName(std::string_view name, int index = 0, std::string_view new_name = "");
	static SafeSharedPtr<Animation> CloneAnimByPath(std::string_view path, int index = 0, std::string_view new_name = "");
	static inline const int GetLoadingCount() { return loading_count; }
	static void Init();
	static void Exit();



};

