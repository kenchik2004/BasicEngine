#pragma once
#include "System/Component.h"


struct AnimationData {
	std::string name;
	std::string path;
	int handle = -1;
	int animation_index = 0;
	float anim_total_time = 0;
};
struct ModelData {
	std::string name;
	std::string path;
	int handle = -1;
};
class ModelRenderer :
	public Component
{
public:
	USING_SUPER(ModelRenderer);
	int Init() override;
	void Load(std::string_view path, std::string_view name = "");
	void SetAnimation(std::string_view name, std::string_view path, int index);
	void Update() override;
	void LateUpdate() override;
	void PostPhysics() override;
	void Exit() override;
	void Draw() override;
	void PlayAnimation(std::string_view name, bool loop = false,float start_time=0.0f);
	void PlayAnimationNoSame(std::string_view name, bool loop = false, float start_time = 0.0f);
	void DebugDraw() override;
	static void UnLoad();

	Vector3 pos = { 0,0,0 };
	Quaternion rot = { 0,0,0,1 };
	Vector3 scale = { 1,1,1 };

	float anim_time = 0;
	bool anim_loop = false;

	ModelData model;
	bool cloned = false;
	std::vector<AnimationData> animation;
	AnimationData* current_anim = nullptr;
	int current_index = -1;

	static std::vector<AnimationData> anim_pool;
	static std::vector<ModelData> model_pool;
};

