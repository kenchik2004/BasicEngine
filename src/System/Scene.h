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

	//TODO Object���珊���V�[���ւ̃|�C���^�ɃA�N�Z�X�ł���@�\�̍쐬

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
	// Init�u���b�N(����������)
	//-----------------------------
	inline virtual int Init() { return 0; }
	//-----------------------------


	//-----------------------------
	// Physics�u���b�N(�����O�㏈��)
	//-----------------------------
	inline virtual void PrePhysics() {}
	void Physics();
	inline virtual void PostPhysics() {}
	void DeleteActor(physx::PxRigidActor* actor);
	void DeleteShape(physx::PxShape* shape);
	//-----------------------------

	//-----------------------------
	// Update�u���b�N(�X�V�O�㏈��)
	//-----------------------------
	inline virtual void PreUpdate() {}
	inline virtual void Update() {}
	inline virtual void LateUpdate() {}
	inline virtual void PostUpdate() {}
	//-----------------------------

	//-----------------------------
	// Draw�u���b�N(�`��O�㏈��)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void Draw() {}
	inline virtual void LateDraw() {}
	//�f�o�b�O�p�`��(�f�o�b�O�E�B���h�E�ɕ`�悳���)
	inline virtual void DebugDraw() {}
	inline virtual void LateDebugDraw() {}

	//�����̏����͕`��Ɏ��t���[���܂Ŕ��f����Ȃ�
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


