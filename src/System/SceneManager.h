#pragma once

USING_PTR(Scene);
USING_PTR(DontDestroyOnLoadScene);
class SceneManager final
{


	//===============================//
	//デバッグ用ボックス(SooS提供)のハンドル
	static int debug_box;
	//===============================//

public:
	//-----------------------------
	// Initブロック(初期化処理)
	//-----------------------------
	static int Init();
	//-----------------------------

	//-----------------------------
	// Physicsブロック(物理前後処理)
	//-----------------------------
	static void PrePhysics();
	static void Physics();
	static void PostPhysics();
	//-----------------------------

	//-----------------------------
	// Updateブロック(更新前後処理)
	//-----------------------------
	static void PreUpdate();
	static void Update();
	static void LateUpdate();
	static void PostUpdate();
	//-----------------------------

	//-----------------------------
	// Drawブロック(描画前後処理)
	//-----------------------------
	static void PreDraw();
	static void Draw();
	static void LateDraw();
	static void DebugDraw();
	static void LateDebugDraw();

	//ここの処理は次フレームまで反映されない
	static void PostDraw();

	static void Exit();

	static inline SceneP GetDontDestoryOnLoadScene() { return another_scenes[0]; }
	template<class T> static inline std::shared_ptr<T> GetScene()
	{
		ClassTypeInfo<T> info = ClassTypeInfo<T>(typeid(T).name(), &T::Super::info);

		std::shared_ptr<T> scene_pick = nullptr;
		std::vector<SceneP> all_scenes = scenes;
		for (auto& another_scene : another_scenes)
			all_scenes.push_back(another_scene);
		for (auto& ite : all_scenes) {
			if (ite->status.ClassName() == info.ClassName())
				scene_pick = std::static_pointer_cast<T>(ite);

		}
		return scene_pick;

	}
	static inline SceneP GetCurrentScene() {
		return current_scene;
	}

	template<class T> static inline void Change(std::shared_ptr<T> scene)
	{
		//シーンの切り替え
		if (!current_scene) {
			current_scene = scene;
			scene->Init();
			//ロード中にdeltatimeが蓄積し、物理がぶっ壊れることがあるため時飛ばし
			Time::ResetTime();
			return;
		}

		current_scene->Exit();
		current_scene->Destroy();
		current_scene = scene;
		scene->Init();
		//ロード中にdeltatimeが蓄積し、物理がぶっ壊れることがあるため時飛ばし
		Time::ResetTime();

	}
	template <class T> static inline void Destroy(std::shared_ptr<T> destroy_scene) {


		for (auto scene = scenes.begin(); scene != scenes.end();) {
			if ((*scene) == destroy_scene) {
				(*scene)->Exit();
				(*scene)->UnLoad();
				try {
					(*scene)->Destroy();
					(*scene)->DestroyPhysics();
				}
				catch (Exception& ex) {
					ex.Show();
					break;
				}
				scene = scenes.erase(scene);
				if (destroy_scene != current_scene)
					destroy_scene.reset();
				break;
			}
		}
		if (destroy_scene == current_scene) {
			current_scene.reset();
		}

	}
	template<class T> static inline std::shared_ptr<T> Load()
	{
		//シーンの作成(いたらロードの必要なし)
		if (std::shared_ptr<T> scene = GetScene<T>()) {
			Change(std::static_pointer_cast<Scene>(scene));
			return scene;

		}

		//(いなかったら、ロードの必要あり)
		auto scene = std::make_shared<T>();
		scene->Construct<T>();
		scene->Load();
		scenes.push_back(std::static_pointer_cast<Scene>(scene));

		Change(std::static_pointer_cast<Scene>(scene));

		return scene;
	}
#if 1
	class Object {
	public:


		template<class T>
		static inline std::shared_ptr<T> Create()
		{
			if (!current_scene)
				return nullptr;
			std::shared_ptr<T> obj = current_scene->CreateObject<T>(typeid(T).name() + 6);
			obj->Init();

			return obj;
		}

		template<class T>
		static inline std::shared_ptr<T> Create(std::string_view name_)
		{
			if (!current_scene)
				return nullptr;
			std::shared_ptr<T> obj = current_scene->CreateObject<T>(name_);
			obj->Init();

			return obj;
		}



		template<class T> static inline std::shared_ptr<T> Get(SceneP target_scene = nullptr) {
			if (!current_scene)
				return nullptr;

			std::shared_ptr<T> obj;
			if (target_scene)
			{
				obj = target_scene->GetObjectPtr<T>();
				return obj;
			}

			obj = current_scene->GetObjectPtr<T>();
			return obj;
		}

		template<class T> static inline std::shared_ptr<T> GetWithTag(ObjBase::TAG tag) {
			if (!current_scene)
				return nullptr;
			std::shared_ptr<T> obj = current_scene->GetObjectPtr<T>(tag);
			return obj;
		}
		template<class T> static inline std::shared_ptr<T> Get(std::string_view name_) {
			if (!current_scene)
				return nullptr;
			std::shared_ptr<T> obj = current_scene->GetObjectPtr<T>(name_);
			return obj;
		}



		template<class T> static inline std::vector<std::shared_ptr<T>> GetArray() {
			std::vector<std::shared_ptr<T>> vec(0);
			if (!current_scene)
				return vec;
			vec = current_scene->GetObjectPtrVec<T>();
			return vec;
		}


		template<class T> static inline std::vector<std::shared_ptr<T>> GetArrayWithTag(ObjBase::TAG tag) {
			std::vector<std::shared_ptr<T>> vec(0);
			if (!current_scene)
				return vec;
			vec = current_scene->GetObjectPtrVec<T>(tag);
			return vec;
		}
		static inline void Destory(ObjBaseP destroy_obj) {
			if (!current_scene)
				return;
			current_scene->DestroyObject(destroy_obj);
			destroy_obj.reset();
		}
		static void DontDestroyOnLoad(ObjBaseP obj, SceneP from_where);
	};
#endif

private:
	static ScenePVec scenes;
	static ScenePVec another_scenes;
	static SceneP current_scene;
};

