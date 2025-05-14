#pragma once
#include "System/Component.h"
#include "System/ModelManager.h"




//!< TO_DO 自前シェーダーに対応

class ModelRenderer :
	public Component
{
public:

	USING_SUPER(ModelRenderer);
	int Init() override;
	void SetModel(std::string_view name_, std::string_view new_name_ = "");
	void Update() override;
	void LateUpdate() override;
	void PreDraw() override;
	void Exit() override;
	void Draw() override;
	void DebugDraw() override;

	static inline void Load(std::string_view path, std::string_view name) {
		ModelManager::LoadAsModel(path, name);
	}

	const int GetModelHandle() { return model ? model->handle : -1; }
	bool IsLoaded() { return model ? model->handle >= 0 : false; }

	std::string model_name = "";
	std::string new_name = "";


	//!<IDEA モデルは、data/modelフォルダに格納しているものすべてを、マネージャ初期化時にロードし、キャッシュしておく
	//! ↑この場合、モデルの名前付けはどうするか...
	//void Load(std::string_view path, std::string_view name = "");


	//モデルコンポーネントが管理するべきもの
	SafeSharedPtr<Model> model;
	Vector3 pos = { 0,0,0 };
	Quaternion rot = { 0,0,0,1 };
	Vector3 scale = { 1,1,1 };




};

//DXLib::MATRIXでは、PhysXの行列と互換性がないので、スケール情報を破棄した上で行と列を入れ替えて取得
inline mat4x4 MV1GetFrameLocalWorldMatrix(int MHandle, int FrameIndex, bool is_physx) {
	mat4x4 mat = cast(MV1GetFrameLocalWorldMatrix(MHandle, FrameIndex));
	mat = CastPhysXMat(mat);
	return mat;
}
