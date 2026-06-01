//---------------------------------------------------------------------------
//! @file   Scene.h
//! @brief  シーン基底クラス。ゲームオブジェクトの管理・更新・描画処理を担う
//---------------------------------------------------------------------------
#pragma once

USING_PTR(Object);
USING_PTR(AudioListener);
USING_PTR(Scene);
USING_PTR(Camera);
//---------------------------------------------------------------------
//! @brief シーン内のクラス名と状態を保持する構造体
//---------------------------------------------------------------------
struct SceneStat {
	friend class Scene;
	std::string ClassName() { return class_name; }
private:
	std::string class_name = "Scene";
};

//---------------------------------------------------------------------
//! @class Scene
//! @brief ゲームシーンの基底クラス。オブジェクトのライフサイクル管理・更新・描画・物理演算を統括する
//---------------------------------------------------------------------
class Scene :public std::enable_shared_from_this<Scene>
{
private:
	physx::PxScene* physics_scene = nullptr; //!< PhysX の物理シーンへのポインタ
public:

	Scene() { physics_scene = PhysicsManager::AddScene(); }
	inline void SetObjPriority(unsigned int new_priority, ObjectP who) {
		unsigned int parent_prio = who->transform->parent ? who->transform->parent->owner->GetPriority() : 0;
		new_priority += parent_prio;
		who->status.priority = new_priority;
		if (std::find(dirty_priority_objects.begin(), dirty_priority_objects.end(), who) == dirty_priority_objects.end())
			dirty_priority_objects.push_back(who);
	}
	// シーンのロード状態を表す列挙型
	enum class LOADING_STATUS :unsigned char {
		LOADING,
		LOADED,
		UNLOADED
	};
	LOADING_STATUS loading_status = LOADING_STATUS::LOADING; //!< 現在のロード状態
	virtual ~Scene() {}

	float physics_timescale = 1.0f; //!< 物理演算のタイムスケール
	inline physx::PxScene* GetPhysicsScene() { return physics_scene; }
	inline bool RayCast(const Ray& ray, RayCastInfo& info,
		const physx::PxQueryFilterData& filter_data = physx::PxQueryFilterData())
	{
		physics_scene->raycast(ray.position, ray.direction, ray.length, info, physx::PxHitFlag::eDEFAULT, filter_data, RaycastCallback::instance());
		return info.hasAnyHits();
	}
	//TODO GameObjectから所属シーンへのポインタにアクセスできる機構の作成

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
	void MoveGameObjectPtrFromThis(ObjectP move_object, SceneP to_where);

	void SetCurrentAudioListener(AudioListenerP listener) { current_audio_listener = listener; }
	AudioListenerWP GetCurrentAudioListener() { return current_audio_listener; }
	void SetCurrentCamera(CameraP camera) { current_camera = camera; }
	CameraWP GetCurrentCamera() { return current_camera; }
	CameraWP& GetCurrentCameraRef() { return current_camera; }

	void RegisterActiveCamera(CameraWP camera);
	void UnregisterActiveCamera(CameraWP camera);
	CameraWPVec& GetActiveCamerasRef() { return active_cameras; }


private:
	bool in_simulation = false;  //!< 物理シミュレーション実行中かどうかのフラグ
	ObjectPVec objects;  //!< シーン内のゲームオブジェクトリスト(優先度順)
	ObjectPVec dirty_priority_objects;  //!< 優先度変更のあったオブジェクトリスト(次フレームで再ソートされる)
	std::vector<std::function<void()>> waiting_functions;  //!< シミュレーション終了後に呼び出す関数リスト
	std::vector<physx::PxActor*> waiting_remove_actors;  //!< シミュレーション後に削除待ちのアクターリスト
	std::vector<physx::PxShape*> waiting_remove_shapes;  //!< シミュレーション後に削除待ちのシェイプリスト
	//std::vector<std::function<void()>> draw_calls;//ドローコールをキャッシュしておく設計
	ObjectWPVec leak_objects;  //!< メモリリーク検出用の弱参照リスト
	void SyncGameObjectsPriority();
	size_t FindInsertPositionByPriority(unsigned int priority);
	bool is_any_destroyed = false;  //!< 削除マーク済みオブジェクトが存在するかどうかのフラグ
	void DestroyMarkedGameObjects();
	AudioListenerWP current_audio_listener;  //!< 現在アクティブなオーディオリスナー
	CameraWP current_camera;  //!< 現在のメインカメラ
	CameraWPVec active_cameras;  //!< アクティブなカメラリスト

protected:

	template<class T, typename...Args>
	inline SafeSharedPtr<T> CreateGameObject(std::string_view name_, Args&&... args)
	{
		auto obj = make_safe_shared<T>(std::forward<Args>(args)...);
		obj->Construct(SafeSharedPtr(shared_from_this()));
		//dirty_priority_objects.push_back(obj);
		size_t insert_pos = FindInsertPositionByPriority(obj->status.priority);
		objects.insert(objects.begin() + insert_pos, obj);
			obj->name = name_.data();
			obj->Init();
			return obj;
		if (!GetGameObjectPtr<Object>(name_)) {
		}

		int i = 1;
		while (true) {
			std::string name(name_);
			name += '_' + std::to_string(i);
			if (!GetGameObjectPtr<Object>(name)) {
				obj->name = name;
				break;
			}
			i++;
		};

		obj->Init();
		return obj;
	}
	template<class T>
	inline SafeSharedPtr<T> CreateGameObjectFromPtr(ObjectP obj, std::string_view name_)
	{
		//登録済みは流石にできないのであきらめろ...(MoveGameObjectPtrFromThisを使ってください)
		if (obj->status.status_bit.is(ObjStat::STATUS::CONSTRUCTED))
			return nullptr;
		obj->Construct(SafeSharedPtr(shared_from_this()));
		dirty_priority_objects.push_back(obj);
		objects.push_back(obj);

		if (!GetGameObjectPtr<Object>(name_)) {
			obj->name = name_.data();
			obj->Init();
			return obj;
		}

		int i = 1;
		while (true) {
			std::string name(name_);
			name += '_' + std::to_string(i);
			if (!GetGameObjectPtr<Object>(name)) {
				obj->name = name;
				break;
			}
			i++;
		};

		obj->Init();
		return obj;
	}
	template<class T> inline SafeSharedPtr<T> GetGameObjectPtr() {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED))
				if (auto pick_obj = SafeDynamicCast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}

	template<class T> inline SafeSharedPtr<T> GetGameObjectPtr(Object::TAG tag) {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->tag == tag)
				if (auto pick_obj = SafeDynamicCast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}

	template<class T> inline SafeSharedPtr<T> GetGameObjectPtr(std::string_view name_) {
		for (auto& obj : objects) {
			if (!obj->status.status_bit.is(ObjStat::STATUS::REMOVED) && obj->name == name_)
				if (auto pick_obj = SafeDynamicCast<T>(obj)) {
					return pick_obj;
				}
		}
		return nullptr;
	}


	template<class T> inline std::vector<SafeSharedPtr<T>> GetGameObjectPtrVec() {
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
	template<class T> inline std::vector<SafeSharedPtr<T>> GetGameObjectPtrVec(Object::TAG tag) {
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

	void DestroyGameObject(ObjectP& destroy_obj);

};


