#pragma once

USING_PTR(ObjBase);
struct SceneStat {
	friend class Scene;
	std::string ClassName() { return class_name; }
private:
	std::string class_name = "Scene";
};

class Scene :public std::enable_shared_from_this<Scene>
{
public:



	//TODO Objectから所属シーンへのポインタにアクセスできる機構の作成

	friend class SceneManager;
	USING_SUPER(Scene);
	SceneStat status;
	template <class T> void Construct() {
		auto class_ = std::static_pointer_cast<T>(shared_from_this());
		class_->status.class_name = typeid(T).name();
	}
	inline virtual void Load() {}

	//-----------------------------
	// Initブロック(初期化処理)
	//-----------------------------
	inline virtual int Init() { return 0; }
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
	// Physicsブロック(物理前後処理)
	//-----------------------------
	inline virtual void PrePhysics() {}
	inline virtual void Physics() {}
	inline virtual void PostPhysics() {}
	//-----------------------------

	//-----------------------------
	// Drawブロック(描画前後処理)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void Draw() {}
	inline virtual void LateDraw() {}
	//デバッグ用描画(デバッグウィンドウに描画される)
	inline virtual void DebugDraw() {}

	//ここの処理は描画に次フレームまで反映されない
	inline virtual void PostDraw() {}

	inline virtual void Exit() {}
	virtual void UnLoad(){}
	void Destory();
private:
	ObjBasePVec objects;

	//template <class T> std::shared_ptr<T> CreateObject();

	template<class T>
	inline std::shared_ptr<T> CreateObject(std::string_view name_)
	{
		auto obj = std::make_shared<T>();
		obj->Construct<T>();
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

	template<class T> inline std::shared_ptr<T> GetObjectPtr() {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED))
				if (auto pick_obj = std::dynamic_pointer_cast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}

	template<class T> inline std::shared_ptr<T> GetObjectPtr(ObjBase::TAG tag) {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->tag == tag)
				if (auto pick_obj = std::dynamic_pointer_cast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}

	template<class T> inline std::shared_ptr<T> GetObjectPtr(std::string_view name_) {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->name == name_)
				if (auto pick_obj = std::dynamic_pointer_cast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}


	template<class T> inline std::vector<std::shared_ptr<T>> GetObjectPtrVec() {
		std::vector<std::shared_ptr<T>> vec(0);

		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED))
				if (auto pick_obj = std::dynamic_pointer_cast<T>(obj))
				{
					vec.push_back(pick_obj);
				}
		}
		return vec;
	}
	template<class T> inline std::vector<std::shared_ptr<T>> GetObjectPtrVec(ObjBase::TAG tag) {
		std::vector<std::shared_ptr<T>> vec(0);

		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->tag == tag)
				if (auto pick_obj = std::dynamic_pointer_cast<T>(obj))
				{
					vec.push_back(pick_obj);
				}
		}
		return vec;
	}

	inline void DestroyObject(ObjBaseP destroy_obj) {
		if (objects.size() <= 0)
			return;
		for (auto obj = objects.begin(); obj != objects.end();) {
			if ((*obj) == destroy_obj) {
				(*obj)->Exit();
				obj = objects.erase(obj);
				destroy_obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
				destroy_obj.reset();
				return;
			}
			obj++;
		}

	}

};


