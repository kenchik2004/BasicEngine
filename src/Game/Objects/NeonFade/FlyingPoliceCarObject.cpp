#include "FlyingPoliceCarObject.h"

namespace NeonFade {

	static const std::array<Vector3, HEADLIGHT_COUNT> HEADLIGHT_OFFSETS = {
		Vector3(0.5f, 0.0f, 1.0f), // 右ヘッドライトのオフセット
		Vector3(-0.5f, 0.0f, 1.0f) // 左ヘッドライトのオフセット
	};

	int FlyingPoliceCarObject::Init()
	{
		// ヘッドライトの生成
		auto light_manager = SceneManager::Object::Get<LightManager>(GetScene());
		if (!light_manager)
			return -1;

		// ヘッドライトを初期化
		for (int i = 0; i < HEADLIGHT_COUNT; ++i)
		{
			Vector3 light_position = transform->position + HEADLIGHT_OFFSETS[i];
			auto light = make_safe_shared<PointLight>(light_position, Color(100.0f, 0.0f, 0.0f), 1.0f, 50.0f);
			if (!light)
				return -1;
			light_manager->AddLight(light);

			head_lights[i] = light;
		}

		// 移動経路の生成
		movement_path = std::make_unique<CatmullRomPath>();

		return 0;
	}
	void FlyingPoliceCarObject::Update()
	{}
	void FlyingPoliceCarObject::Exit()
	{}
}