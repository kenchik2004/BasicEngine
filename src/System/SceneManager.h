#pragma once

USING_PTR(Scene);
USING_PTR(DontDestroyOnLoadScene);
//@brief �V�[���}�l�W�����g�N���X
class SceneManager final
{


	//===============================//
	//�f�o�b�O�p�{�b�N�X(SooS��)�̃n���h��
	static int debug_box;
	//===============================//

public:
	//-----------------------------
	// Init�u���b�N(����������)
	//-----------------------------

	static int Init();		//<������

	//-----------------------------

	//-----------------------------
	// Physics�u���b�N(�����O�㏈��)
	//-----------------------------

	static void PrePhysics();		//<�����O�X�V
	static void Physics();			//<�����X�V
	static void PostPhysics();		//<������X�V

	//-----------------------------

	//-----------------------------
	// Update�u���b�N(�X�V�O�㏈��)
	//-----------------------------

	static void PreUpdate();		//<�O�X�V
	static void Update();			//<�X�V
	static void LateUpdate();		//<�x���X�V
	static void PostUpdate();		//<��X�V

	//-----------------------------

	//-----------------------------
	// Draw�u���b�N(�`��O�㏈��)
	//-----------------------------

	static void PreDraw();			//<�O�`��������͕`��O�X�V
	static void Draw();				//<�`��
	static void LateDraw();			//<�x���`��
	static void DebugDraw();		//<�f�o�b�O�`��
	static void LateDebugDraw();	//<��f�o�b�O�`��

	//�����̏����͎��t���[���܂Ŕ��f����Ȃ�
	static void PostDraw();			//<�t���[�����ŏI�X�V

	static void Exit();			//<�I��


	//���̃f�t�H���g�V�[�����擾
	static inline SceneP GetDontDestoryOnLoadScene() { return another_scenes[0]; }

	//����̃V�[�����擾
	template<class T> static inline SafeSharedPtr<T> GetScene()
	{
		ClassTypeInfo<T> info = ClassTypeInfo<T>(typeid(T).name(), &T::Super::info);

		SafeSharedPtr<T> scene_pick = nullptr;
		//���V�[�����܂߂�
		std::vector<SceneP> all_scenes = scenes;
		//������ɗ��V�[����ǉ�
		for (auto& another_scene : another_scenes)
			all_scenes.push_back(another_scene);

		//�쐬���̃N���X������v����V�[��������
		for (auto& ite : all_scenes) {
			if (ite->status.ClassName() == info.ClassName())
				scene_pick = SafeStaticCast<T>(ite);

		}
		return scene_pick;

	}

	//���݂̕\�V�[�����擾
	static inline SceneP GetCurrentScene() {
		return current_scene;
	}

	template<class T> static inline void Change(SafeSharedPtr<T> scene)
	{
		//�V�[���̐؂�ւ�
		//�J�����g�V�[�������Ȃ��Ȃ炻�̂܂܃��[�h
		if (!current_scene) {
			current_scene = scene;
			scene->Init();
			//���[�h����deltatime���~�ς��A�������Ԃ����邱�Ƃ����邽�ߎ���΂�
			Time::ResetTime();
			return;
		}
		//�J�����g�V�[��������ꍇ�͏I�����ă��[�h
		current_scene->Exit();
		current_scene->Destroy();
		current_scene = scene;
		scene->Init();
		//���[�h����deltatime���~�ς��A�������Ԃ����邱�Ƃ����邽�ߎ���΂�
		Time::ResetTime();

	}

	//����̃V�[�������S�ɔj��
	template <class T> static inline void Destroy(SafeSharedPtr<T> destroy_scene) {


		//�V�[��������
		for (auto scene = scenes.begin(); scene != scenes.end();) {
			//����������j��
			if ((*scene) == destroy_scene) {
				(*scene)->Exit();
				(*scene)->UnLoad();

				//�V�[����j������ہA���[�N�����m���邽��try->catch
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
		//�j���V�[�����J�����g�V�[���Ȃ�A�Q�Ƃ�j��
		if (destroy_scene == current_scene) {
			current_scene.reset();
		}

	}

	//�V�[�������[�h(�쐬)
	template<class T> static inline SafeSharedPtr<T> Load()
	{
		//�V�[���̍쐬(�����烍�[�h�̕K�v�Ȃ�)
		if (SafeSharedPtr<T> scene = GetScene<T>()) {
			//�w��̃V�[���ɕύX
			Change(SafeStaticCast<Scene>(scene));
			//�ύX��V�[���̃|�C���^��Ԃ�
			return scene;

		}

		//(���Ȃ�������A���[�h�̕K�v����)
		auto scene = make_safe_shared<T>();
		//�V�[���̍쐬&�o�^
		scene->Construct<T>();
		//���[�h�f�[�^������Ȃ烍�[�h
		scene->Load();
		//�V�[���̔z��ɓo�^
		scenes.push_back(SafeStaticCast<Scene>(scene));
		//�V�[���ύX
		Change(SafeStaticCast<Scene>(scene));

		return scene;
	}

	//�I�u�W�F�N�g�֌W�̑���p�N���X
	class Object {
	public:

		//�I�u�W�F�N�g�̍쐬
		template<class T>
		static inline SafeSharedPtr<T> Create()
		{
			if (!current_scene)
				return nullptr;
			SafeSharedPtr<T> obj = current_scene->CreateObject<T>(typeid(T).name() + 6);

			obj->Init();

			return obj;
		}

		//�I�u�W�F�N�g�̍쐬(���t��)
		template<class T>
		static inline SafeSharedPtr<T> Create(std::string_view name_)
		{
			//�J�����g�V�[�������Ȃ��Ȃ牽���������^�[��
			if (!current_scene)
				return nullptr;
			//�J�����g�V�[���ɃI�u�W�F�N�g�쐬���˗�
			SafeSharedPtr<T> obj = current_scene->CreateObject<T>(name_);
			//�쐬�����I�u�W�F�N�g�����̏�ŏ�����
			obj->Init();

			//��������̃I�u�W�F�N�g��Ԃ�
			return obj;
		}


		//�I�u�W�F�N�g�擾(�V�[���w���)
		template<class T> static inline SafeSharedPtr<T> Get(SceneP target_scene = nullptr) {

			//�J�����g�V�[�����^�[�Q�b�g�V�[�������Ȃ��ꍇ�Anullptr��Ԃ�
			if (!current_scene && !target_scene)
				return nullptr;


			SafeSharedPtr<T> obj = nullptr;
			//�V�[���w������Ă���ꍇ
			if (target_scene)
			{
				//���̃V�[������I�u�W�F�N�g����������
				obj = target_scene->GetObjectPtr<T>();

				return obj;
			}
			//�w�肪�Ȃ��ꍇ�̓J�����g�V�[�����猟��
			obj = current_scene->GetObjectPtr<T>();
			return obj;
		}

		//�I�u�W�F�N�g���^�O���猟��
		template<class T> static inline SafeSharedPtr<T> GetWithTag(ObjBase::TAG tag) {
			//�J�����g�V�[�������Ȃ��Ȃ�nullptr��Ԃ�
			if (!current_scene)
				return nullptr;
			//�J�����g�V�[������I�u�W�F�N�g������
			SafeSharedPtr<T> obj = current_scene->GetObjectPtr<T>(tag);

			//���������I�u�W�F�N�g��Ԃ�
			return obj;
		}

		//�I�u�W�F�N�g�𖼑O���猟��
		template<class T> static inline SafeSharedPtr<T> Get(std::string_view name_) {
			//�J�����g�V�[�������Ȃ��Ȃ�nullptr��Ԃ�
			if (!current_scene)
				return nullptr;
			//�J�����g�V�[������I�u�W�F�N�g������
			SafeSharedPtr<T> obj = current_scene->GetObjectPtr<T>(name_);

			//���������I�u�W�F�N�g��Ԃ�
			return obj;
		}


		//�����^�̃I�u�W�F�N�g���܂Ƃ߂Ď擾
		template<class T> static inline std::vector<SafeSharedPtr<T>> GetArray() {

			//����ۂ̔z���p��
			std::vector<SafeSharedPtr<T>> vec(0);
			//�J�����g�V�[�������Ȃ��Ȃ�A��̂܂ܕԂ�
			if (!current_scene)
				return vec;

			//�J�����g�V�[������A�܂Ƃ߂Č���
			vec = current_scene->GetObjectPtrVec<T>();

			//�����������̂�Ԃ�
			return vec;
		}


		//�����^�̃I�u�W�F�N�g���܂Ƃ߂Č���
		template<class T> static inline std::vector<SafeSharedPtr<T>> GetArrayWithTag(ObjBase::TAG tag) {

			//����ۂ̔z���p��
			std::vector<SafeSharedPtr<T>> vec(0);
			//�J�����g�V�[�������Ȃ��Ȃ�A����ۂ̂܂ܕԂ�
			if (!current_scene)
				return vec;

			//�J�����g�V�[������܂Ƃ߂Č���
			vec = current_scene->GetObjectPtrVec<T>(tag);

			//�����������̂�Ԃ�
			return vec;
		}

		//�I�u�W�F�N�g���폜
		static inline void Destory(ObjBaseP destroy_obj) {
			//�J�����g�V�[�������Ȃ��Ȃ牽�����Ȃ�
			if (!current_scene)
				return;

			//�J�����g�V�[������I�u�W�F�N�g���폜
			current_scene->DestroyObject(destroy_obj);
			//�폜��A�Q�Ƃ��폜
			destroy_obj.reset();
		}

		//�V�[���؂�ւ����ɔj�����Ȃ��I�u�W�F�N�g��o�^(�f�t�H���g���V�[���ɏ��L�����n)
		static void DontDestroyOnLoad(ObjBaseP obj, SceneP from_where);
	};


private:
	static ScenePVec scenes;			//!<�쐬�ς݃V�[���̔z��
	static ScenePVec another_scenes;	//!<���V�[���̔z��
	static SceneP current_scene;		//!<���݃V�[��(�J�����g�V�[��)�ւ̃|�C���^
};

