#include "precompile.h"
#include "StageMakeScene.h"


namespace NeonFade_Debug {
	//NeonFade_Debug::StageMakeScene
	int input_handle = -1;
	bool is_input = false;

	int StageMakeScene::Init()
	{
		camera = SceneManager::Object::Create<GameObject>();
		auto cam_comp = camera->AddComponent<Camera>();
		input_handle = MakeKeyInput(1024, true, false, false);
		return 0;
	}
	void StageMakeScene::Update()
	{
		{
			Vector3 mov = { 0,0,0 };
			if (Input::GetKey(KeyCode::W))
				mov.z += 1.0f;
			if (Input::GetKey(KeyCode::S))
				mov.z -= 1.0f;
			if (Input::GetKey(KeyCode::D))
				mov.x += 1.0f;
			if (Input::GetKey(KeyCode::A))
				mov.x -= 1.0f;
			if (Input::GetKey(KeyCode::Q))
				mov.y += 1.0f;
			if (Input::GetKey(KeyCode::E))
				mov.y -= 1.0f;
			Vector2 rot = Input::GetMouseDelta();
			mov = mov.getNormalized() * Time::DeltaTime();
			TransformP& trns = camera->transform;
			if (Input::GetKey(KeyCode::LShift))
				mov *= 5;
			trns->position += mov.x * trns->AxisX();
			trns->position += mov.y * trns->AxisY();
			trns->position += mov.z * trns->AxisZ();
			trns->SetAxisX(ProjectOnPlane(trns->AxisX(), { 0,1,0 }), trns->AxisY());

			if (Input::GetMouseButtonRepeat(MouseButton::ButtonRight)) {
				Quaternion rot_quat = EulerToQuaternion({ DEG2RAD(rot.y),DEG2RAD(rot.x),0 });
				trns->rotation *= rot_quat;
			}

		}
		{
			if (!is_input && Input::GetKeyDown(KeyCode::Return)) {
				SetActiveKeyInput(input_handle);
				is_input = true;
			}
			if (is_input && CheckKeyInput(input_handle) == 1) {
				is_input = false;
				std::array<char, 1024> buf;
				buf.fill('\0');
				GetKeyInputString(&buf[0], input_handle);
				std::string input_str = buf.data();
				SetKeyInputString("\0", input_handle);
				current_obj = CreateNewObject(input_str);
			}
			if (is_input && CheckKeyInput(input_handle) == 2)
			{
				is_input = false;
			}
		}
	}
	void StageMakeScene::LateDebugDraw()
	{
		if (!CheckKeyInput(input_handle)) {
			DrawKeyInputString(0, 32, input_handle);
		}

	}
	void StageMakeScene::Exit()
	{
		camera.reset();
		current_obj.reset();
	}
	GameObjectP StageMakeScene::CreateNewObject(const std::string& mdl_path)
	{
		return factory.CreateObject(shared_from_this(), mdl_path);
	}
	GameObjectP StageMakeScene::ObjFactory::CreateObject(const SceneP scene, const std::string& mdl_path)
	{
		GameObjectP obj = SceneManager::Object::Create<GameObject>(scene);
		if (!obj)
			return nullptr;
		auto mdl = obj->AddComponent<ModelRenderer>();
		std::string model_name = "";
		if (mdl_path.find('/') == mdl_path.npos)
			model_name = mdl_path;
		else
			model_name = mdl_path.substr(mdl_path.rfind('/'));

		if (model_name.empty()) {
			SceneManager::Object::Destroy(obj);
			return nullptr;
		}
		ModelManager::LoadAsModel(mdl_path, model_name);
		auto mdl_loaded = ModelManager::CloneModelByName(model_name);
		if (!mdl_loaded) {
			SceneManager::Object::Destroy(obj);
			return nullptr;
		}
		mdl_loaded->use_lighting = false;
		mdl->SetModel(mdl_loaded);
		mdl->cast_shadow = false;
		return obj;

	}
}
