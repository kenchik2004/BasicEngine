#pragma once

USING_PTR(ObjBase);
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
	virtual ~Scene() {}

	float physics_timescale = 1.0f;
	inline physx::PxScene* GetPhysicsScene() { return physics_scene; }

	//TODO Objectから所属シーンへのポインタにアクセスできる機構の作成

	friend class SceneManager;
	USING_SUPER(Scene);
	SceneStat status;
	template <class T> void Construct() {
		auto class_ = std::static_pointer_cast<T>(shared_from_this());
		class_->status.class_name = typeid(T).name();
		physics_scene = PhysicsManager::AddScene();
	}
	inline virtual void Load() {}

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
	inline virtual void Update() {}
	inline virtual void LateUpdate() {}
	inline virtual void PostUpdate() {}
	//-----------------------------

	//-----------------------------
	// Drawブロック(描画前後処理)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void Draw() {}
	inline virtual void LateDraw() {}
	//デバッグ用描画(デバッグウィンドウに描画される)
	inline virtual void DebugDraw() {}
	inline virtual void LateDebugDraw() {}

	//ここの処理は描画に次フレームまで反映されない
	inline virtual void PostDraw() {}

	inline virtual void Exit() {}
	virtual void UnLoad() {}
	void Destroy();
	void DestroyPhysics();
	inline const bool& IsInSimulation() { return in_simulation; }
	inline void AddFunctionAfterSimulation(const std::function<void()> function) { waiting_functions.push_back(function); }
	void MoveObjectPtrFromThis(ObjBaseP move_object, SceneP to_where);
private:
	bool in_simulation = false;
	ObjBasePVec objects;
	std::vector<std::function<void()>> waiting_functions;
	std::vector<physx::PxActor*> waiting_remove_actors;
	std::vector<physx::PxShape*> waiting_remove_shapes;
	ObjBaseWPVec leak_objects;

protected:

	template<class T,typename...Args>
	inline SafeSharedPtr<T> CreateObject(std::string_view name_,Args&&... args)
	{
		auto obj = make_safe_shared<T>(std::forward<Args>(args)...);
		obj->Construct<T>(SafeSharedPtr(shared_from_this()));
		objects.push_back(obj);
		ObjBase::changed_priority = true;

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


