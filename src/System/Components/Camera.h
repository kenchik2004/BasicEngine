//---------------------------------------------------------------------------
//! @file   Camera.h
//! @brief  3Dカメラのビュー・投影行列管理を行うCameraコンポーネント
//---------------------------------------------------------------------------
#pragma once



namespace CreateMatrix {

	//---------------------------------------------------------------------------
	//!  [左手座標系] ビュー行列
	//---------------------------------------------------------------------------
	mat4x4 lookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& worldUp);

	//---------------------------------------------------------------------------
	//! [左手座標系] 平行投影行列
	//---------------------------------------------------------------------------
	mat4x4 orthographicOffCenterLH(f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z);

	//---------------------------------------------------------------------------
	//! [左手座標系] 投影行列
	//---------------------------------------------------------------------------
	mat4x4 perspectiveFovLH(f32 fovy, f32 aspect_ratio, f32 near_z, f32 far_z);
}

//! @brief スカイボックスのリソースをセットアップする
void SetUpSkyboxResources(const SafeSharedPtr<Texture>& default_texture = nullptr);
//! @brief スカイボックスのテクスチャを設定する
void SetSkyboxTexture(const SafeSharedPtr<Texture>& texture);
//! @brief スカイボックスのリソースを解放する
void ReleaseSkyboxResources();

USING_PTR(Camera);
//---------------------------------------------------------------------
//! @class Camera
//! @brief 3Dシーンの撮影・ビュー/投影行列の管理を行うカメラコンポーネント
//---------------------------------------------------------------------
class Camera :
	public Component
{
private:
	bool is_current_camera = false;       //!< 現在アクティブなカメラか
	int constant_buffer_handle = -1;      //!< カメラ定数バッファのハンドル
public:
	//! @brief コンストラクト処理
	void Construct() override;
	//! @brief 初期化処理
	int Init() override;
	//! @brief 描画処理
	void Draw() override;
	//! @brief 後処理描画
	void LateDraw() override;
	//! @brief カメラパラメータを準備する（オーバーライド可能）
	virtual void PrepareCamera();
	//! @brief カメラの定数バッファを設定する
	void SetCameraConstantBuffer();
	//! @brief このカメラをアクティブカメラに設定する
	void SetCurrentCamera();
	//! @brief 終了処理
	void Exit() override;
	//! @brief 透視投影の視野角を設定する
	inline void SetPerspective(float perspective_) { perspective = perspective_; SetupCamera_Perspective(DEG2RAD(perspective)); }
	//! @brief 現在の視野角を取得する
	inline float GetPerspective() { return perspective; }
	//! @brief GBufferテクスチャをスロットにバインドする
	void SetGbufferToSlot() const;
	//! @brief 現在アクティブなCameraを取得する
	SafeSharedPtr<Camera> GetCurrentCamera();
	SafeSharedPtr<Texture> hdr = nullptr;   //!< HDRレンダリング用テクスチャ
	SafeSharedPtr<Texture> depth = nullptr; //!< 深度テクスチャ

	//! @brief レンダリング方式の種別
	enum class RenderType {
		Forward,  //!< フォワードレンダリング
		Deferred, //!< ディファードレンダリング
	};
	//! @brief 画面クリア方式の種別
	enum class ClearType {
		SkyBox, //!< スカイボックスでクリア
		Color,  //!< 単色でクリア
	};
	RenderType render_type = RenderType::Forward; //!< レンダリング方式
	ClearType clear_type = ClearType::SkyBox;     //!< クリア方式
	Color clear_color = { 0,0,0,0 };              //!< クリアカラー（ClearType::Color時に使用）
	// ■【GBufferのレイアウト】■
//            R         G         B         A
//       +---------+---------+---------+---------+
// RT0   |          Albedo.rgb         |    AO   |  DXGI_FORMAT_R8G8B8A8_UNORM (Full-rate)
//       +---------+---------+---------+---------+
// RT1   |Normal.xyz(圧縮済み)|Roughness| Metallic|  DXGI_FORMAT_R8G8B8A8_UNORM (Full-rate)
//       +---------+---------+---------+---------+
// RT2   |      WorldPosition.xyz      |/////////|  DXI_FORMAT_R32G32B32A32_FLOAT (1/4-rate)
//       +---------+---------+---------+---------+
//
//       +---------+---------+---------+---------+
// Depth |                 Depth                 |  DXGI_FORMAT_D32_FLOAT
//       +---------+---------+---------+---------+
	static constexpr u32 GBUFFER_NUM = 4;    //!< GBufferの数

	std::array<SafeSharedPtr<Texture>, GBUFFER_NUM> gbuffer_texture_; //!< GBufferテクスチャ配列

	float camera_near = 0.1f;   //!< ニアクリップ距離
	float camera_far = 2000.0f; //!< ファークリップ距離
	float perspective = 45.0f;  //!< 透視投影の視野角（度）
};

#if 0
class DebugCamera :
	public Camera
{
public:
	void Construct() override;
	int Init() override;
	void Update() override;
	void PreDraw() override;
	void PrepareCamera() override;
	void Exit() override;
};
#endif