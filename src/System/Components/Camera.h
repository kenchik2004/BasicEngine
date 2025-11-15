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

void SetUpSkyboxResources(const SafeSharedPtr<Texture>& default_texture = nullptr);
void SetSkyboxTexture(const SafeSharedPtr<Texture>& texture);
void ReleaseSkyboxResources();

USING_PTR(Camera);
class Camera :
	public Component
{
private:
	bool is_current_camera = false;
	int constant_buffer_handle = -1;
public:
	void Construct() override;
	int Init() override;
	void Draw() override;
	void LateDraw() override;
	virtual void PrepareCamera();
	void SetCameraConstantBuffer();
	void SetCurrentCamera();
	void Exit() override;
	inline void SetPerspective(float perspective_) { perspective = perspective_; SetupCamera_Perspective(DEG2RAD(perspective)); }
	inline float GetPerspective() { return perspective; }
	void SetGbufferToSlot() const;
	SafeSharedPtr<Camera> GetCurrentCamera();
	SafeSharedPtr<Texture> hdr = nullptr;
	SafeSharedPtr<Texture> depth = nullptr;

	enum class RenderType {
		Forward,
		Deferred,
	};
	RenderType render_type = RenderType::Forward;

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

	std::array<SafeSharedPtr<Texture>, GBUFFER_NUM> gbuffer_texture_;

	float camera_near = 0.1f;
	float camera_far = 2000.0f;
	float perspective = 45.0f;
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