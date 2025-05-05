#pragma once
#include "System/Component.h"

#define CUR_ANIMTIME_MAX FLT_MAX

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

//!< TO_DO モデルのデータを構造体化->分離、アニメーションデータに関しても同様
//!< TO_DO 自前シェーダーに対応

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
	void PreDraw() override;
	void Exit() override;
	void Draw() override;
	void PlayAnimation(std::string_view name, bool loop = false, float start_time = 0.0f);
	void PlayAnimationNoSame(std::string_view name, bool loop = false, float start_time = 0.0f);
	std::string_view GetCurrentAnimName();
	void DebugDraw() override;
	bool IsPlaying();
	static void UnLoad();


	Vector3 pos = { 0,0,0 };
	Quaternion rot = { 0,0,0,1 };
	Vector3 scale = { 1,1,1 };

	float anim_time = 0;
	float anim_speed = 1.0f;
	bool anim_loop = false;

	ModelData model;
	bool cloned = false;
	std::vector<AnimationData> animation;
	AnimationData* current_anim = nullptr;
	int current_index = -1;

	static std::vector<AnimationData> anim_pool;
	static std::vector<ModelData> model_pool;
};

//DXLib::MATRIXでは、PhysXの行列と互換性がないので、スケール情報を破棄した上で行と列を入れ替えて取得
inline mat4x4 MV1GetFrameLocalWorldMatrix(int MHandle, int FrameIndex, bool is_physx) {
	mat4x4 mat = cast(MV1GetFrameLocalWorldMatrix(MHandle, FrameIndex));
	mat = CastPhysXMat(mat);
	return mat;
}
