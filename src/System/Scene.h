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



	//TODO Object���珊���V�[���ւ̃|�C���^�ɃA�N�Z�X�ł���@�\�̍쐬

	friend class SceneManager;
	USING_SUPER(Scene);
	SceneStat status;
	template <class T> void Construct() {
		auto class_ = std::static_pointer_cast<T>(shared_from_this());
		class_->status.class_name = typeid(T).name();
	}
	inline virtual void Load() {}

	//-----------------------------
	// Init�u���b�N(����������)
	//-----------------------------
	inline virtual int Init() { return 0; }
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
	// Physics�u���b�N(�����O�㏈��)
	//-----------------------------
	inline virtual void PrePhysics() {}
	inline virtual void Physics() {}
	inline virtual void PostPhysics() {}
	//-----------------------------

	//-----------------------------
	// Draw�u���b�N(�`��O�㏈��)
	//-----------------------------
	inline virtual void PreDraw() {}
	inline virtual void Draw() {}
	inline virtual void LateDraw() {}
	//�f�o�b�O�p�`��(�f�o�b�O�E�B���h�E�ɕ`�悳���)
	inline virtual void DebugDraw() {}

	//�����̏����͕`��Ɏ��t���[���܂Ŕ��f����Ȃ�
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


