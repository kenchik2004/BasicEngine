#pragma once

USING_PTR(ObjBase);
USING_PTR(AudioListener);
USING_PTR(Scene);
struct SceneStat {
	friend class Scene;
	std::string ClassName() { return class_name; }
private:
	std::string class_name = "Scene";
};

class Scene :public std::enable_shared_from_this<Scene>
{
private:
	physx::PxScene* physics_scene = nullptr;
public:

	Scene() { physics_scene = PhysicsManager::AddScene(); }
	inline void SetObjPriority(int new_priority, ObjBaseP who) {
		who->status.priority = new_priority;
		dirty_priority_objects.push_back(who);
	}
	enum class LOADING_STATUS :unsigned char {
		LOADING,
		LOADED,
		UNLOADED
	};
	LOADING_STATUS loading_status = LOADING_STATUS::LOADING;
	virtual ~Scene() {}

	float physics_timescale = 1.0f;
	inline physx::PxScene* GetPhysicsScene() { return physics_scene; }

	//TODO Objectから所属シーンへのポインタにアクセスできる機構の作成

	friend class SceneManager;
	USING_SUPER(Scene);
	SceneStat status;
	void Construct() {
		status.class_name = Info()->ClassName();
	}
	inline virtual void Load() { loading_status = LOADING_STATUS::LOADED; }

	//-----------------------------
	// Initブロック(初期化処理)
	//-----------------------------
	inline virtual int Init() { return 0; }
	//-----------------------------


	//-----------------------------
	// Physicsブロック(物理前後処理)
	//-----------------------------
	inline virtual void PrePhysics() {}
	void Physics();
	inline virtual void PostPhysics() {}
	void DeleteActor(physx::PxRigidActor* actor);
	void DeleteShape(physx::PxShape* shape);
	//-----------------------------

	//-----------------------------
	// Updateブロック(更新前後処理)
	//-----------------------------
	inline virtual void PreUpdate() {}
	inline virtual void OnPreUpdateFinish() {}
	inline virtual void Update() {}
	inline virtual void OnUpdateFinish() {}
	inline virtual void LateUpdate() {}
	inline virtual void OnLateUpdateFinish() {}
	inline virtual void PostUpdate() {}
	inline virtual void OnPostUpdateFinish() {}
	//-----------------------------

	//-----------------------------
	// Drawブロック(描画前後処理)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void OnPreDrawFinish() {}
	inline virtual void Draw() {}
	inline virtual void OnDrawFinish() {}
	inline virtual void LateDraw() {}
	inline virtual void OnLateDrawFinish() {}
	//デバッグ用描画(デバッグウィンドウに描画される)
	inline virtual void DebugDraw() {}
	inline virtual void OnDebugDrawFinish() {}
	inline virtual void LateDebugDraw() {}
	inline virtual void OnLateDebugDrawFinish() {}

	//ここの処理は描画に次フレームまで反映されない
	inline virtual void PostDraw() {}
	inline virtual void OnPostDrawFinish() {}

	inline virtual void Exit() {}
	virtual void UnLoad() {}
	void Destroy();
	void DestroyPhysics();
	inline const bool& IsInSimulation() { return in_simulation; }
	inline void AddFunctionAfterSimulation(const std::function<void()> function) { waiting_functions.push_back(function); }
	void MoveObjectPtrFromThis(ObjBaseP move_object, SceneP to_where);

	void SetCurrentAudioListener(ComponentP listener) { current_audio_listener = listener; }
	ComponentWP GetCurrentAudioListener() { return current_audio_listener; }
	void SetCurrentCamera(ComponentP camera) { current_camera = camera; }
	ComponentWP GetCurrentCamera() { return current_camera; }


private:
	bool in_simulation = false;
	ObjBasePVec objects;
	ObjBasePVec dirty_priority_objects;
	std::vector<std::function<void()>> waiting_functions;
	std::vector<physx::PxActor*> waiting_remove_actors;
	std::vector<physx::PxShape*> waiting_remove_shapes;
	ObjBaseWPVec leak_objects;
	void SyncObjectsPriority();
	ComponentWP current_audio_listener;
	ComponentWP current_camera;

protected:

	template<class T, typename...Args>
	inline SafeSharedPtr<T> CreateObject(std::string_view name_, Args&&... args)
	{
		auto obj = make_safe_shared<T>(std::forward<Args>(args)...);
		obj->Construct<T>(SafeSharedPtr(shared_from_this()));
		dirty_priority_objects.push_back(obj);
		objects.push_back(obj);

		if (!GetObjectPtr<ObjBase>(name_)) {
			obj->name = name_.data();
			return obj;
		}

		int i = 1;
		while (true) {
			std::string name(name_);
			name += std::to_string(i);
			if (!GetObjectPtr<ObjBase>(name)) {
				obj->name = name;
				break;
			}
			i++;
		};

		return obj;
	}

	template<class T> inline SafeSharedPtr<T> GetObjectPtr() {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED))
				if (auto pick_obj = SafeDynamicCast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}

	template<class T> inline SafeSharedPtr<T> GetObjectPtr(ObjBase::TAG tag) {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->tag == tag)
				if (auto pick_obj = SafeDynamicCast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}

	template<class T> inline SafeSharedPtr<T> GetObjectPtr(std::string_view name_) {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->name == name_)
				if (auto pick_obj = SafeDynamicCast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}


	template<class T> inline std::vector<SafeSharedPtr<T>> GetObjectPtrVec() {
		std::vector<SafeSharedPtr<T>> vec(0);

		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED))
				if (auto pick_obj = SafeDynamicCast<T>(obj))
				{
					vec.push_back(pick_obj);
				}
		}
		return vec;
	}
	template<class T> inline std::vector<SafeSharedPtr<T>> GetObjectPtrVec(ObjBase::TAG tag) {
		std::vector<SafeSharedPtr<T>> vec(0);

		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->tag == tag)
				if (auto pick_obj = SafeDynamicCast<T>(obj))
				{
					vec.push_back(pick_obj);
				}
		}
		return vec;
	}

	void DestroyObject(ObjBaseP destroy_obj);

};


