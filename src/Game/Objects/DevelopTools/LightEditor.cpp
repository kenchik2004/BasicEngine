#include "LightEditor.h"
#include "System/Objects/ShadowMapObject.h"

Vector3 FromString(const std::string& str) {
	Vector3 vec = {
		std::stof(str.substr(0, str.find(","))),
		std::stof(str.substr(str.find(",") + 1, str.rfind(",") - str.find(",") - 1)),
		std::stof(str.substr(str.rfind(",") + 1))
	};

	return vec;
};

class LightObject :public GameObject
{
public:
	USING_SUPER(LightObject);

	void SetLight(const SafeSharedPtr<LightBase>& light) {
		my_light = light;

		// ライトが無効な場合はオブジェクトを削除する
		if (!my_light)
			SceneManager::Object::Destroy(shared_from_this());
	}

	const SafeSharedPtr<LightBase>& GetLight() const {
		return my_light;
	}

	void Update() override {

		light_info.position = transform->position;		//< ライトの位置をオブジェクトの位置に合わせる
		light_info.direction = transform->AxisZ();		//< ライトの照射方向をオブジェクトの前方に合わせる



		my_light->position = light_info.position;

		//色を反映する
		my_light->color = light_info.color;


		//ライトの種類に応じて、範囲や強度、方向などの情報を反映する
		if (my_light->type == LightType::Point) {
			// ポイントライトの場合は、範囲と強度を反映する
			// ライトの型を安全にキャストして、ポイントライト固有のプロパティにアクセスする
			auto point_light = SafeStaticCast<PointLight>(my_light);

			point_light->range = light_info.range;				//< ライトの影響範囲を反映
			point_light->intensity = light_info.intensity;		//< ライトの強度を反映
		}
		if (my_light->type == LightType::Directional) {
			// ディレクショナルライトの場合は、照射方向を反映する
			// ライトの型を安全にキャストして、ディレクショナルライト固有のプロパティにアクセスする
			auto dir_light = SafeStaticCast<DirectionalLight>(my_light);

			dir_light->direction = light_info.direction;		//< ライトの照射方向を反映
		}

	}

	void SetColor(const Color& color) {
		light_info.color = color;
	}
	void SetRange(float range) {
		light_info.range = range;
	}
	void SetIntensity(float intensity) {
		light_info.intensity = intensity;
	}

	const Color GetColor() const {
		return light_info.color;
	}

	const float& GetRange() const {
		return light_info.range;
	}
	const float& GetIntensity() const {
		return light_info.intensity;
	}

	void Exit() override {
		// ライトオブジェクトが削除されるときに、ライトの参照をリセットする
		if (my_light)
			my_light.reset();
	}

private:
	SafeSharedPtr<LightBase> my_light;		//!<ライトの実体
	LightInfo light_info; //!< ライトの情報 (位置、色、範囲、強度など)
};


int LightEditor::Init()
{
	// ライトマネージャーが存在しない場合は作成する
	light_manager = SceneManager::Object::Get<LightManager>();
	if (!light_manager)
		SceneManager::Object::Create<LightManager>("LightManager");
	auto& lights = light_manager->GetLights();
	for (auto& light : lights) {
		CreateLightObject(light);
	}

	return Super::Init();
}

void LightEditor::Update()
{

	if (Input::GetKey(KeyCode::LControl) && Input::GetKeyDown(KeyCode::S)) {
		Save(file_path);
	}
	if (Input::GetKey(KeyCode::LControl) && Input::GetKeyDown(KeyCode::F)) {
		Load(file_path);
	}

	//Lキーでライトを新規作成
	if (Input::GetKeyDown(KeyCode::L)) {
		static const Color def_light_color = Color(10.0f, 10.0f, 10.0f);
		static const float def_light_range = 10.0f;
		static const float def_light_intensity = 1.0f;

		// デバッグカメラが存在する場合は、前方にポイントライトを追加する	
		auto dbg_camera = SceneManager::Object::Get<GameObject>("DebugCamera");

		if (dbg_camera) {
			Vector3 light_pos = dbg_camera->transform->position;
			light_pos += dbg_camera->transform->AxisZ() * 5.0f; // カメラの前方に5ユニット移動

			// ライトを追加
			AddLight(LightType::Point, light_pos, def_light_color, def_light_range, def_light_intensity);
		}
		// デバッグカメラが存在しない場合は、とりあえずワールドの原点にポイントライトを追加する
		else {
			// ライトを追加
			AddLight(LightType::Point, Vector3(0, 0, 0), def_light_color, def_light_range, def_light_intensity);
		}
	}
	// 選択されているライトがある場合は、そのライトの情報を更新する
	if (light_objects.empty())
		return;

	// 選択されているライトの情報を更新する
	EditLight(light_objects[selected_index].lock(), edit_speed, color_speed);

	// スラッシュキーで次のライトを選択、乗算キーで前のライトを選択
	if (Input::GetKeyDown(KeyCode::Slash))
		selected_index++;
	if (Input::GetKeyDown(KeyCode::Multipuly))
		selected_index--;

	// ライトの数に応じて、選択されているライトのインデックスをループさせる
	selected_index = (selected_index + light_objects.size()) % light_objects.size();


	// デリートキーで選択されているライトを削除する
	if (Input::GetKeyDown(KeyCode::Delete))
		DeleteLight(light_objects[selected_index].lock());







}

void LightEditor::DebugDraw()
{
#ifndef NDEBUG
	return;
#endif
	if (light_objects.empty())
		return;

	auto selected_light = light_objects[selected_index].lock();
	if (!selected_light)
		return;

	DrawSphere3D(cast(selected_light->transform->position), selected_light->GetRange(), 16, selected_light->GetColor(), selected_light->GetColor(), false);

}

void LightEditor::Save(std::string_view path)
{
	if (!light_data_file || !light_data_file->is_open())
		light_data_file = std::make_unique<std::fstream>(path.data(), std::ios::out);


	const std::vector<SafeSharedPtr<LightBase>>& lights = light_manager->GetLights();
	for (auto& light : lights) {
		std::string output;
		output += "{\n";

		output += "    type:" + std::to_string(static_cast<int>(light->type)) + ",\n";
		output += "    position:{" + std::to_string(light->position.x) + "," + std::to_string(light->position.y) + "," + std::to_string(light->position.z) + "},\n";
		output += "    direction:{" + std::to_string(light->type == LightType::Directional ? static_cast<DirectionalLight*>(light.get())->direction.x : 0) + "," + std::to_string(light->type == LightType::Directional ? static_cast<DirectionalLight*>(light.get())->direction.y : 0) + "," + std::to_string(light->type == LightType::Directional ? static_cast<DirectionalLight*>(light.get())->direction.z : 0) + "},\n";
		output += "    color:{" + std::to_string(light->color.r) + "," + std::to_string(light->color.g) + "," + std::to_string(light->color.b) + "},\n";
		output += "    range:" + std::to_string(light->type == LightType::Point ? static_cast<PointLight*>(light.get())->range : 0) + ",\n";
		output += "    intensity:" + std::to_string(light->type == LightType::Point ? static_cast<PointLight*>(light.get())->intensity : 0) + ",\n";

		output += "}\n";
		light_data_file->write(output.c_str(), output.size());
	}
	light_data_file->flush();
	light_data_file->close();
}

void LightEditor::Load(std::string_view path)
{

	while (light_objects.size() > 0)
	{
		DeleteLight(light_objects.front().lock());
	}

	light_data_file = std::make_unique<std::fstream>(path.data(), std::ios::in);

	while (light_data_file->good())
	{
		int scope = 0;
		std::string light_data_str;
		do {
			std::string line;
			line.reserve(256);
			char* buffer = line.data();
			light_data_file->getline(buffer, line.max_size());
			line = buffer;

			size_t offset = 0;
			while (line.find("{", offset) != line.npos) {
				scope++;
				offset = line.find("{", offset) + 1;
			}
			offset = 0;
			while (line.find("}", offset) != line.npos) {
				scope--;
				offset = line.find("}", offset) + 1;
			}
			light_data_str += line;

		} while (scope > 0);
		if (light_data_str.empty())
			continue;

		// 読み取ったライトデータの文字列から、ライトの情報を解析してLightInfo構造体に格納する
																//タイプ	　　  位置　　　　  照射方向　　  　色　　　　 範囲　　　　 強度
		static const std::array<std::string, 6> light_params = { "type:", "position:{", "direction:{", "color:{", "range:", "intensity:" };
		LightInfo light_info;
		//ライトタイプを読み取る
		{
			size_t type_start = light_data_str.find(light_params[0]) + light_params[0].size();
			size_t type_end = light_data_str.find(",", type_start);
			std::string type_buffer = light_data_str.substr(type_start, type_end - type_start);
			light_info.type = std::stoi(type_buffer);
		}

		//位置を読み取る
		{
			size_t pos_start = light_data_str.find(light_params[1]) + light_params[1].size();
			size_t pos_end = light_data_str.find("},", pos_start + 1);
			std::string pos_buffer = light_data_str.substr(pos_start, pos_end - pos_start);
			light_info.position = FromString(pos_buffer);
		}

		//照射方向を読み取る
		{
			size_t dir_start = light_data_str.find(light_params[2]) + light_params[2].size();
			size_t dir_end = light_data_str.find("},", dir_start + 1);
			std::string dir_buffer = light_data_str.substr(dir_start, dir_end - dir_start);
			light_info.direction = FromString(dir_buffer).getNormalized();
		}

		//色を読み取る
		{
			size_t color_start = light_data_str.find(light_params[3]) + light_params[3].size();
			size_t color_end = light_data_str.find("},", color_start + 1);
			std::string color_buffer = light_data_str.substr(color_start, color_end - color_start);
			light_info.color = FromString(color_buffer);
		}

		//範囲を読み取る
		{
			size_t range_start = light_data_str.find(light_params[4]) + light_params[4].size();
			size_t range_end = light_data_str.find(",", range_start);
			std::string range_buffer = light_data_str.substr(range_start, range_end - range_start);
			light_info.range = std::stof(range_buffer);
		}

		//強度を読み取る
		{
			size_t intensity_start = light_data_str.find(light_params[5]) + light_params[5].size();
			size_t intensity_end = light_data_str.find(",", intensity_start);
			std::string intensity_buffer = light_data_str.substr(intensity_start, intensity_end - intensity_start);
			light_info.intensity = std::stof(intensity_buffer);
		}

		// ライトの情報に基づいて、ライトオブジェクトを作成してシーンに追加する
		AddLight(static_cast<LightType>(light_info.type), light_info.position, light_info.color, light_info.range, light_info.intensity, light_info.direction);

		if (light_data_file->eof())
			break;



	}
	light_data_file->close();


}

void LightEditor::AddLight(LightType type, const Vector3& position, const Color& color, float range, float intensity, Vector3 direction)
{
	if (!light_manager)
		return;

	// ライトの種別とパラメーターに応じて、ライトオブジェクトを作成する
	SafeSharedPtr<LightBase> light = nullptr;
	switch (type)
	{
	case LightType::Directional:

		// ディレクショナルライトを作成して、位置、色、照射方向を設定する
		light = make_safe_shared<DirectionalLight>(position, color, direction);
		break;

	case LightType::Point:

		// ポイントライトを作成して、位置、色、範囲、強度を設定する
		light = make_safe_shared<PointLight>(position, color, intensity, range);
		break;

	default:
		return;
	}
	light_manager->AddLight(light);
	if (light->type == LightType::Directional) {
		auto shadow_map_obj = SceneManager::Object::Get<ShadowMapObject>(GetScene());
		if (!shadow_map_obj)
			shadow_map_obj = SceneManager::Object::Create<ShadowMapObject>(GetScene());
		shadow_map_obj->SetLightDirection(direction);
	}

	CreateLightObject(light);

}

void LightEditor::EditLight(LightObjectP edit_obj, float mov_speed, float color_speed)
{
	// テンキーの8,2,4,6でライトオブジェクトを前後左右に移動する
	Vector3 mov = { 0,0,0 };
	if (Input::GetKey(KeyCode::NumPad8))
		mov.z = 1;
	if (Input::GetKey(KeyCode::NumPad2))
		mov.z = -1;
	if (Input::GetKey(KeyCode::NumPad4))
		mov.x = -1;
	if (Input::GetKey(KeyCode::NumPad6))
		mov.x = 1;
	// テンキーの9,3でライトオブジェクトを上下に移動する
	if (Input::GetKey(KeyCode::NumPad9))
		mov.y = 1;
	if (Input::GetKey(KeyCode::NumPad3))
		mov.y = -1;

	// 移動量を正規化して、移動速度を掛け適用する
	mov = mov.getNormalized() * mov_speed;
	edit_obj->transform->position += mov;

	// テンキーの1,7でライトの範囲を増減する
	if (Input::GetKey(KeyCode::NumPad7))
		edit_obj->SetRange(edit_obj->GetRange() + color_speed * Time::DeltaTime());
	if (Input::GetKey(KeyCode::NumPad1))
		edit_obj->SetRange(edit_obj->GetRange() - color_speed * Time::DeltaTime());

	// テンキーの4,6でライトの強度を増減する
	Color current_color = edit_obj->GetColor();
	if (Input::GetKey(KeyCode::NumPad5)) {
		current_color += Color(1.0f, 1.0f, 1.0f) * color_speed * Time::DeltaTime();
	}

	if (Input::GetKey(KeyCode::NumPad0)) {
		current_color -= Color(1.0f, 1.0f, 1.0f) * color_speed * Time::DeltaTime();
	}
	edit_obj->SetColor(current_color);


}

void LightEditor::DeleteLight(LightObjectP light_obj)
{
	if (!light_manager)
		return;
	auto light = light_obj->GetLight();
	SceneManager::Object::Destroy(light_obj);
	for (auto it = light_objects.begin(); it != light_objects.end(); ++it) {
		if (it->lock() == light_obj) {
			light_objects.erase(it);
			break;
		}
	}
	if (light) {
		light_manager->RemoveLight(light);
	}

	if (!light_objects.empty())
		selected_index = std::clamp(selected_index, 0ull, light_objects.size() - 1);
	else
		selected_index = 0ull;

}

void LightEditor::Exit()
{}

void LightEditor::CreateLightObject(const SafeSharedPtr<LightBase>& light)
{
	// ライトオブジェクトを作成して、ライトを設定する
	auto light_obj = SceneManager::Object::Create<LightObject>("LightObject");
	LightType type = light->type;
	Vector3 position = light->position;
	Color color = light->color;
	float range = 0.0f;
	float intensity = 1.0f;
	Vector3 direction = { 0,-1,0 };
	if (type == LightType::Point)
	{
		auto point_light = SafeStaticCast<PointLight>(light);
		range = point_light->range;
		intensity = point_light->intensity;
	}
	if (type == LightType::Directional) {
		auto dir_light = SafeStaticCast<DirectionalLight>(light);
		direction = dir_light->direction;
	}

	light_obj->transform->position = position;
	light_obj->transform->SetAxisZ(direction);
	light_obj->SetColor(color);
	light_obj->SetIntensity(intensity);
	light_obj->SetRange(range);
	light_obj->SetLight(light);
	light_objects.push_back(light_obj);


	selected_index = light_objects.size() - 1;
}
