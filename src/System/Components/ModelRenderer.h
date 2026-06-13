//---------------------------------------------------------------------------
//! @file   ModelRenderer.h
//! @brief  3Dモデルの描画を管理するModelRendererコンポーネント
//---------------------------------------------------------------------------
#pragma once




USING_PTR(ModelRenderer);
//---------------------------------------------------------------------
//! @class ModelRenderer
//! @brief 3Dモデルの読み込みと描画を管理するコンポーネント
//---------------------------------------------------------------------
class ModelRenderer :
	public Component
{
public:

	USING_SUPER(ModelRenderer);
	//! @brief コンストラクト処理
	void Construct() override;
	//! @brief 初期化処理
	int Init() override;
	//! @brief モデルを名前で設定する
	void SetModel(std::string_view name_, std::string_view new_name_ = "");
	//! @brief モデルをポインタで設定する
	void SetModel(SafeSharedPtr<Model> model_, std::string_view new_name_ = "");
	//! @brief 更新処理
	void Update() override;
	//! @brief 遅延更新処理
	void LateUpdate() override;
	//! @brief 物理演算後処理（ボーン行列の更新）
	void PostPhysics() override;
	//! @brief 終了処理
	void Exit() override;
	//! @brief 描画前処理
	void PreDraw() override;
	//! @brief 描画処理
	void Draw() override;
	//! @brief シャドウマップへの描画
	void DrawToShadowMap();
	//! @brief デバッグ描画処理
	void DebugDraw() override;

	//! @brief モデルファイルをロードする
	static void Load(std::string_view path, std::string_view name);

	//! @brief モデルハンドルを取得する
	const int GetModelHandle() { return model ? model->handle : -1; }
	//! @brief 指定フレームのワールド行列をDX形式で取得する
	MATRIX GetFrameWorldMatDX(u32 index);
	//! @brief 指定フレームのワールド行列を取得する
	mat4x4 GetFrameWorldMat(u32 index);
	//! @brief フレーム名でワールド行列をDX形式で取得する
	MATRIX GetFrameWorldMatDX(std::string_view frame_name);
	//! @brief フレーム名でワールド行列を取得する
	mat4x4 GetFrameWorldMat(std::string_view frame_name);
	//! @brief モデルがロード済みか返す
	bool IsLoaded() { return model ? model->handle >= 0 : false; }

	std::string model_name = ""; //!< モデルの識別名
	std::string new_name = "";   //!< モデルの別名


	bool cast_shadow = true; //!< シャドウを投影するか
	//! @brief シャドウ投影フラグを設定する
	void SetCastShadow(bool enable) { cast_shadow = enable; }
	//! @brief シャドウ投影フラグを取得する
	bool CastShadow() { return cast_shadow; }

	//!<IDEA モデルは、data/modelフォルダに格納しているものすべてを、マネージャ初期化時にロードし、キャッシュしておく
	//! ↑この場合、モデルの名前付けはどうするか...


	//モデルコンポーネントが管理するべきもの
	SafeSharedPtr<Model> model; //!< モデルリソースへのポインタ
	//! @brief 指定インデックスのマテリアルを設定する
	void SetMaterial(Material* material, size_t index);
	//! @brief 指定インデックスのマテリアルを取得する
	Material* GetMaterial(size_t index);
	//! @brief マテリアルの数を取得する
	size_t GetMaterialsNum();
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
