#pragma once

enum class LightType :u32 {
	Directional,
	Point,
};
struct LightInfo {
	Vector3 position; //!< 光源の位置座標
	float range; //!< 光源の影響範囲
	Vector3 direction; //!< 光源の向き
	int type;			//!<ライトタイプ
	Vector3 color; //!< 光源の色
	float intensity;	//!< 減衰率

};
class LightBase {
	friend class LightManager;
public:
	virtual ~LightBase() {}
	void Init();
	LightType type;
	Vector3 position;
	Color color;
protected:
	SafeWeakPtr<LightManager> my_manager = nullptr;
	ShaderPs* accumulation_shader = nullptr;
	bool is_initialized = false;
	int cbuffer_handle = -1;
	virtual void DrawToAccumulationBuffer() = 0;
	virtual void SetLightConstantBuffer() = 0;
};

class DirectionalLight :public LightBase
{
public:
	Vector3 direction;
	DirectionalLight() { type = LightType::Directional; direction = { 0,-1,0 }; }
protected:
	void DrawToAccumulationBuffer() override;
	void SetLightConstantBuffer() override;
};

class PointLight :public LightBase
{
public:
	float intensity;
	float range;
	PointLight() { type = LightType::Point; intensity = 1.0f; range = 10.0f; }
protected:
	void DrawToAccumulationBuffer() override;
	void SetLightConstantBuffer() override;
};
class LightManager :public Object
{
private:
	std::vector<SafeSharedPtr<LightBase>> lights;
	ShaderPs* light_blend_shader = nullptr;
	SafeSharedPtr<Texture> specular_accumulation_texture = nullptr;
	SafeSharedPtr<Texture> diffuse_accumulation_texture = nullptr;
	mat4x4 camera_view_proj = mat4x4(physx::PxIdentity);
	int lights_cbuffer_handle = -1;
	static constexpr u8 MAX_FORWARD_LIGHTS = 6;

public:
	LightManager() :Object(1) {}
	USING_SUPER(LightManager);

	int Init() override;
	void AddLight(SafeSharedPtr<LightBase> light) {
		if (!light)
			return;
		if (!light->is_initialized)
			light->Init();
		light->my_manager = SafeStaticCast<LightManager>(SafeSharedPtr(shared_from_this()));
		lights.push_back(light);
	}
	void AddLight(LightType type, const Vector3& position, const Color& color, float range = 0, float intensity = 1.0f, Vector3 direction = { 0,-1,0 }) {
		auto light = SafeSharedPtr<LightBase>(nullptr);
		switch (type) {
		case LightType::Directional: {
			auto dir_light = make_safe_shared<DirectionalLight>();
			dir_light->direction = direction.getNormalized();
			dir_light->color = color;
			light = dir_light;
			break;
		}
		case LightType::Point: {
			auto point_light = make_safe_shared<PointLight>();
			point_light->intensity = intensity;
			point_light->range = range;
			light = point_light;
			break;
		}
		}
		if (!light)
			return;
		light->type = type;
		light->position = position;
		light->color = color;
		if (!light->is_initialized)
			light->Init();
		light->my_manager = SafeStaticCast<LightManager>(SafeSharedPtr(shared_from_this()));
		lights.push_back(light);
	}
	void RemoveLight(SafeSharedPtr<LightBase> light) {
		if (!light)
			return;
		auto ite = std::find(lights.begin(), lights.end(), light);
		if (ite != lights.end()) {
			lights.erase(ite);
		}
	}
	const std::vector<SafeSharedPtr<LightBase>>& GetLights() const {
		return lights;
	}
	void Draw() override;
	void LateDraw() override;
	void LateDebugDraw() override;
	void Exit() override;
	const mat4x4& GetCameraViewProj() const { return camera_view_proj; }
};

