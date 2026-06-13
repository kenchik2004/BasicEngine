#pragma once
#pragma once
#include "Game/Managers/LightManager.h"
#include <fstream>

USING_PTR(LightObject);
class LightEditor :
	public GameObject
{
public:
	USING_SUPER(LightEditor);
	int Init() override;

	void Update() override;
	void DebugDraw() override;

	void Save(std::string_view path);
	void Load(std::string_view path);

	void AddLight(LightType type, const Vector3& position, const Color& color, float range = 0, float intensity = 1.0f, Vector3 direction = { 0,-1,0 });
	void EditLight(LightObjectP edit_obj, float mov_speed = 1.0f, float color_speed = 5.0f);

	void DeleteLight(LightObjectP light_obj);
	void Exit() override;

	float edit_speed = 1.0f; //!< ライト編集の移動速度
	float color_speed = 10.0f; //!< ライト編集の色変更速度
	std::string file_path = "data/LightData.txt"; //!< ライトデータの保存先ファイルパス
private:
	void CreateLightObject(const SafeSharedPtr<LightBase>& light);
	bool is_color_edit = false;
	std::unique_ptr<std::fstream> light_data_file = nullptr;
	size_t selected_index = 0;
	SafeWeakPtr<LightManager> light_manager;
	LightObjectWPVec light_objects;

};

