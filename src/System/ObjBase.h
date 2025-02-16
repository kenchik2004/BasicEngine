#pragma once



struct ObjStat {
	friend class ObjBase;
	friend class Object;
	friend class UIObject;
public:
	enum OBJ_TYPE {
		NORMAL,
		UI,
	};
	enum struct STATUS :u32 {
		INITIALIZED = 0,
		ACTIVE = 1 << 1,
		DRAW = 1 << 2,
		REMOVED = 1 << 3,
	};

	SBit <STATUS> status_bit;
private:
	std::string class_name = "ObjBase";
	OBJ_TYPE obj_type = NORMAL;
	int priority = 0;
};

USING_PTR(Component);
class ObjBase :public std::enable_shared_from_this<ObjBase>
{
	friend class Scene;
	friend class SceneManager;
public:

	USING_SUPER(ObjBase);
	ObjStat status;
	enum TAG {
		Untaged,
		Player,
		Enemy,
		Stage,
		UI,
	};
	TAG tag = Untaged;
	std::string name = "EmptyObject";
private:


	template <class T>
	inline void Construct() {
		auto this_class = std::static_pointer_cast<T>(shared_from_this());
		this_class->status.status_bit.on(ObjStat::STATUS::INITIALIZED);
		this_class->status.status_bit.on(ObjStat::STATUS::ACTIVE);
		this_class->status.status_bit.on(ObjStat::STATUS::DRAW);
		this_class->status.class_name = typeid(T).name();
	};

	static bool changed_priority;
	bool changed_comp_priority = true;
	ComponentPVec components;


public:

	template <class T> std::shared_ptr<T> AddComponent(...) {
		auto comp = std::make_shared<T>();
		comp->owner = shared_from_this();
		comp->Construct<T>();
		components.push_back(comp);
		comp->Init();
		changed_comp_priority = true;
		return comp;
	}
	template <class T> std::shared_ptr<T> GetComponent() {
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				if (auto pick_comp = std::dynamic_pointer_cast<T>(comp)) {
					return pick_comp;
				}
		}
		return nullptr;
	}
	template <class T> std::vector<std::shared_ptr<T>> GetComponents() {
		std::vector<std::shared_ptr<T>> vec(0);
		if (!GetComponent<T>())
			return vec;
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				if (auto pick_comp = std::dynamic_pointer_cast<T>(comp)) {
					vec.push_back(pick_comp);
				}
		}
		return vec;
	}

	void RemoveComponent(ComponentP remove_comp) {
		for (auto comp = components.begin(); comp != components.end();) {
			if ((*comp) == remove_comp) {
				(*comp)->Exit();
				(*comp)->status.status_bit.on(CompStat::STATUS::REMOVED);
				comp = components.erase(comp);
				remove_comp.reset();
				return;
			}
			comp++;
		}
	}

	static inline const bool ChangedPriority() { return changed_priority; }
	inline void SetPriority(int prio) { status.priority = prio; changed_priority = true; }
	inline int GetPriority() { return status.priority; }

	inline void ChangedCompPriority(bool is_changed = true) { changed_comp_priority = is_changed; }
	inline const bool IsChangedCompPriority() { return changed_comp_priority; }

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
	//-----------------------------

	inline virtual void Exit() {}
};


class Object :public ObjBase {
public:
	USING_SUPER(Object);

	Object() { status.obj_type = ObjStat::NORMAL; }
};
class UIObject :public ObjBase {
public:
	USING_SUPER(UIObject);
	UIObject() { status.obj_type = ObjStat::UI; tag = UI; SetPriority(2000); }
};
