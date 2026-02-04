#include "precompile.h"
#include "StageMakeScene.h"
#include <fstream>

namespace NeonFade_Debug {
	//NeonFade_Debug::StageMakeScene
	int input_handle = -1;
	bool is_input = false;
	std::fstream logfile;

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
			Vector2 rot = Input::GetMouseDelta() * Time::DeltaTime() * 30;
			mov = mov.getNormalized() * Time::DeltaTime();
			TransformP& trns = camera->transform;
			if (Input::GetKey(KeyCode::LShift))
				mov *= 5;
			trns->position += mov.x * trns->AxisX();
			trns->position += mov.y * trns->AxisY();
			trns->position += mov.z * trns->AxisZ();
			trns->SetAxisX(ProjectOnPlane(trns->AxisX(), { 0,1,0 }), trns->AxisY());

			if (Input::GetMouseButtonRepeat(MouseButton::ButtonRight)) {
				Quaternion rot_quat = EulerToQuaternion({ rot.y,rot.x,0 });
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
		{
			if (current_obj) {
				auto& trns = current_obj->transform;
				Vector3 mov = { 0,0,0 };
				if (Input::GetKey(KeyCode::Up))
					mov.z += 1.0f;
				if (Input::GetKey(KeyCode::Down))
					mov.z -= 1.0f;
				if (Input::GetKey(KeyCode::Right))
					mov.x += 1.0f;
				if (Input::GetKey(KeyCode::Left))
					mov.x -= 1.0f;
				if (Input::GetKey(KeyCode::RShift))
					mov.y += 1.0f;
				if (Input::GetKey(KeyCode::RControl))
					mov.y -= 1.0f;
				mov = mov.getNormalized() * Time::DeltaTime() * 5;
				trns->position += mov.x * trns->AxisX();
				trns->position += mov.y * trns->AxisY();
				trns->position += mov.z * trns->AxisZ();
				float rot_y = 0;
				if (Input::GetKey(KeyCode::O))
					rot_y += 1.0f;
				if (Input::GetKey(KeyCode::P))
					rot_y -= 1.0f;
				rot_y *= Time::DeltaTime() * 90;
				trns->AddRotation({ 0,rot_y,0 });
				float scale = 1.0f;
				if (Input::GetKey(KeyCode::K))
					scale += Time::DeltaTime();
				if (Input::GetKey(KeyCode::L))
					scale -= Time::DeltaTime() * 0.5f;
				trns->scale = Vector3(scale, scale, scale).multiply(trns->scale);

				if (Input::GetKeyDown(KeyCode::Delete))
					SceneManager::Object::Destroy(current_obj);
			}
		}
	}
	void StageMakeScene::LateDebugDraw()
	{
		if (!CheckKeyInput(input_handle)) {
			DrawKeyInputString(0, 32, input_handle);
		}
		if (current_obj) {
			{
				auto trns = current_obj->transform;
				const auto& pos = trns->position;
				const auto rot = QuaternionToEuler(trns->rotation);
				const auto& scale = trns->scale;
				std::string output = "Object Name: " + current_obj->name + "\n";
				output += "Position: \n";
				output += "\t(" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")\n";
				output += "Rotation: \n";
				output += "\t(" + std::to_string(rot.x) + ", " + std::to_string(rot.y) + ", " + std::to_string(rot.z) + ")\n";
				output += "Scale: \n";
				output += "\t(" + std::to_string(scale.x) + ", " + std::to_string(scale.y) + ", " + std::to_string(scale.z) + ")\n";

				printfDx("%s", output.c_str());
			}
		}


	}
	void StageMakeScene::Exit()
	{
		camera.reset();
		current_obj.reset();
		auto all_objs = SceneManager::Object::GetArray<GameObject>();
		logfile.open("data/LogFile.txt", std::ios::out);
		if (!logfile.is_open())
			return;
		logfile << "Objects" << std::endl;
		for (auto& obj : all_objs) {
			if (obj->GetComponent<Camera>())
				continue;
			logfile << "Object Name: " << obj->name << std::endl;
			auto mdl = obj->GetComponent<ModelRenderer>();
			{
				auto trns = obj->transform;
				logfile << "\t Position: \n\t\t(" << trns->position.x << ", " << trns->position.y << ", " << trns->position.z << ")" << std::endl;
				logfile << "\t Rotation: \n\t\t(" << trns->rotation.x << ", " << trns->rotation.y << ", " << trns->rotation.z << ", " << trns->rotation.w << ")" << std::endl;
				logfile << "\t Scale: \n\t\t(" << trns->scale.x << ", " << trns->scale.y << ", " << trns->scale.z << ")" << std::endl;
			}
			if (mdl) {
				logfile << "\t Model Path: " << mdl->model->GetPath() << std::endl;
			}
			logfile << "------------------------" << std::endl;
		}
		logfile.close();

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
