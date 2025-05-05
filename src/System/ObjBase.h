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
USING_PTR(Transform);
USING_PTR(ObjBase);
USING_PTR(Scene);
class ObjBase :public std::enable_shared_from_this<ObjBase>
{
	friend class Scene;
	friend class SceneManager;
public:
	virtual ~ObjBase() {}

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
	TransformP transform = nullptr;
private:


	template <class T>
	inline void Construct(SceneP owner_scene) {
		auto this_class = std::static_pointer_cast<T>(shared_from_this());
		this_class->status.status_bit.on(ObjStat::STATUS::INITIALIZED);
		this_class->status.status_bit.on(ObjStat::STATUS::ACTIVE);
		this_class->status.status_bit.on(ObjStat::STATUS::DRAW);
		this_class->status.class_name = typeid(T).name();
		this_class->transform = this_class->AddComponent<Transform>();
		this_class->scene = owner_scene;
	};

	static bool changed_priority;
	bool changed_comp_priority = true;
	ComponentPVec components;
	SceneP scene;


public:
	template <class T,typename... Args> SafeSharedPtr<T> AddComponent(Args&& ...args) {

		auto comp = make_safe_shared<T>(std::forward<Args>(args)...);
		comp->owner = SafeSharedPtr(shared_from_this());
		comp->Construct<T>();
		components.push_back(comp);
		comp->Init();
		changed_comp_priority = true;
		return comp;
	}
	template <class T> SafeSharedPtr<T> GetComponent() {
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				if (auto pick_comp = SafeDynamicCast<T>(comp)) {
					return pick_comp;
				}
		}
		return nullptr;
	}
	template <class T> std::vector<SafeSharedPtr<T>> GetComponents() {
		std::vector<SafeSharedPtr<T>> vec(0);
		if (!GetComponent<T>())
			return vec;
		for (auto& comp : components) {
			if (!comp->status.status_bit.is(CompStat::STATUS::REMOVED))
				if (auto pick_comp = SafeDynamicCast<T>(comp)) {
					vec.push_back(pick_comp);
				}
		}
		return vec;
	}

	void RemoveComponent(ComponentP remove_comp) {
		ComponentWP comp_wp;
		for (auto comp = components.begin(); comp != components.end();) {
			comp_wp = (*comp);
			if (comp_wp.lock() == remove_comp) {
				comp_wp.lock()->Exit();
				comp_wp.lock()->status.status_bit.on(CompStat::STATUS::REMOVED);
				comp = components.erase(comp);
				remove_comp.reset();
				break;

			}
			comp++;
		}
	}

	static inline const bool ChangedPriority() { return changed_priority; }
	inline void SetPriority(int prio) { status.priority = prio; changed_priority = true; }
	inline int GetPriority() { return status.priority; }
	inline SceneP GetScene() { return scene; }

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
	// Hit時コールバック
	//-----------------------------
	inline virtual void OnCollisionEnter(const HitInfo& hit_info) {}
	inline virtual void OnCollisionStay(const HitInfo& hit_info) {}
	inline virtual void OnCollisionExit(const HitInfo& hit_info) {}

	//-----------------------------
	// Hit時コールバック(トリガー)
	//-----------------------------
	inline virtual void OnTriggerEnter(const HitInfo& hit_info) {}
	inline virtual void OnTriggerStay(const HitInfo& hit_info) {}
	inline virtual void OnTriggerExit(const HitInfo& hit_info) {}
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
	//-----------------------------

	inline virtual void Exit() {}
};

USING_PTR(Object);
class Object :public ObjBase {
public:
	USING_SUPER(Object);
	Object();
	void DebugDraw() override;


};


USING_PTR(UIObject);
class UIObject :public ObjBase {
public:
	USING_SUPER(UIObject);
	UIObject();
};
