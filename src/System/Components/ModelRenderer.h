#pragma once




USING_PTR(ModelRenderer);
class ModelRenderer :
	public Component
{
public:

	USING_SUPER(ModelRenderer);
	void Construct() override;
	int Init() override;
	void SetModel(std::string_view name_, std::string_view new_name_ = "");
	void SetModel(SafeSharedPtr<Model> model_, std::string_view new_name_ = "");
	void Update() override;
	void LateUpdate() override;
	void PostPhysics() override;
	void Exit() override;
	void PreDraw() override;
	void Draw() override;
	void DrawToShadowMap();
	void DebugDraw() override;

	static void Load(std::string_view path, std::string_view name);

	const int GetModelHandle() { return model ? model->handle : -1; }
	MATRIX GetFrameWorldMatDX(u32 index);
	mat4x4 GetFrameWorldMat(u32 index);
	MATRIX GetFrameWorldMatDX(std::string_view frame_name);
	mat4x4 GetFrameWorldMat(std::string_view frame_name);
	bool IsLoaded() { return model ? model->handle >= 0 : false; }

	std::string model_name = "";
	std::string new_name = "";


	bool cast_shadow = true;
	void SetCastShadow(bool enable) { cast_shadow = enable; }
	bool CastShadow() { return cast_shadow; }

	//!<IDEA モデルは、data/modelフォルダに格納しているものすべてを、マネージャ初期化時にロードし、キャッシュしておく
	//! ↑この場合、モデルの名前付けはどうするか...


	//モデルコンポーネントが管理するべきもの
	SafeSharedPtr<Model> model;
	void SetMaterial(Material* material, size_t index);
	Material* GetMaterial(size_t index);
	//Vector3 pos = { 0,0,0 };
	//Quaternion rot = { 0,0,0,1 };
	//Vector3 scale = { 1,1,1 };




};

//DXLib::MATRIXでは、PhysXの行列と互換性がないので、スケール情報を破棄した上で行と列を入れ替えて取得
inline mat4x4 MV1GetFrameLocalWorldMatrix(int MHandle, int FrameIndex, bool is_physx) {
	mat4x4 mat = cast(MV1GetFrameLocalWorldMatrix(MHandle, FrameIndex));
	mat = CastPhysXMat(mat);
	return mat;
}
