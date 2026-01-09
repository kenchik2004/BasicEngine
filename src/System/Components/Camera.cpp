#include "Camera.h"
#include "System/Scene.h"
#include <System/Objects/ShadowMapObject.h>

namespace {
	SafeSharedPtr<Model> sky_dome = nullptr;
	SafeSharedPtr<Texture> sky_texture = nullptr;
}


namespace CreateMatrix {

	//---------------------------------------------------------------------------
	//!  [左手座標系] ビュー行列
	//---------------------------------------------------------------------------
	mat4x4 lookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& worldUp)
	{
		Vector3 axis_z = (lookAt - eye).getNormalized();
		Vector3 axis_x = worldUp.cross(axis_z).getNormalized();
		Vector3 axis_y = axis_z.cross(axis_x);

		f32 tx = -axis_x.dot(eye);
		f32 ty = -axis_y.dot(eye);
		f32 tz = -axis_z.dot(eye);

		Vector4 m[4]{
			{ axis_x.x, axis_y.x, axis_z.x, 0.0f },
			{ axis_x.y, axis_y.y, axis_z.y, 0.0f },
			{ axis_x.z, axis_y.z, axis_z.z, 0.0f },
			{ tx,       ty,       tz, 1.0f },
		};
		return mat4x4(m[0], m[1], m[2], m[3]);
	}

	//---------------------------------------------------------------------------
	//! [左手座標系] 平行投影行列
	//---------------------------------------------------------------------------
	mat4x4 orthographicOffCenterLH(f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z)
	{
		float rcp_width = 1.0f / (right - left);
		float rcp_height = 1.0f / (top - bottom);
		float range = 1.0f / (far_z - near_z);

		Vector4 m[4]{
			{ rcp_width * 2.0f,                         0.0f,            0.0f, 0.0f },
			{ 0.0f,            rcp_height * 2.0f,            0.0f, 0.0f },
			{ 0.0f,                         0.0f,           range, 0.0f },
			{ -(left + right) * rcp_width, -(top + bottom) * rcp_height, -range * near_z, 1.0f }
		};
		return mat4x4(m[0], m[1], m[2], m[3]);
	}

	//---------------------------------------------------------------------------
	//! [左手座標系] 投影行列
	//---------------------------------------------------------------------------
	mat4x4 perspectiveFovLH(f32 fovy, f32 aspect_ratio, f32 near_z, f32 far_z)
	{
		f32 s = std::sinf(fovy * 0.5f);
		f32 c = std::cosf(fovy * 0.5f);

		f32 height = c / s;
		f32 width = height / aspect_ratio;
		f32 range = far_z / (far_z - near_z);

		Vector4 m[4]{
			{ width,   0.0f,            0.0f, 0.0f },
			{ 0.0f, height,            0.0f, 0.0f },
			{ 0.0f,   0.0f,           range, 1.0f },
			{ 0.0f,   0.0f, -range * near_z, 0.0f }
		};
		return mat4x4(m[0], m[1], m[2], m[3]);
	}
	//---------------------------------------------------------------------------
	//! [左手座標系] 投影逆行列
	//---------------------------------------------------------------------------
	mat4x4 InverseperspectiveFovLH(f32 fovy, f32 aspect_ratio, f32 near_z, f32 far_z)
	{
		f32 s = std::sinf(fovy * 0.5f);
		f32 c = std::cosf(fovy * 0.5f);

		f32 height = c / s;
		f32 width = height / aspect_ratio;

		f32 range = far_z / (far_z - near_z);


		Vector4 m[4]{
			{ 1.0f / width, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f / height, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, -1.0f / (range * near_z) },
			{ 0.0f, 0.0f, 1.0f , 1.0f / near_z }
		};
		return mat4x4(m[0], m[1], m[2], m[3]);
	}

}

struct CBufferCameraInfo {
	mat4x4 view;
	mat4x4  proj;
	mat4x4 view_inv;
	mat4x4  proj_inv;

	Vector4 camera_pos;
	mat4x4 mat_light_view_proj_[4];    //!< ライトのビュー投影行列
};


void Camera::Construct()
{
	SetPriority(1);
	status.status_bit.on(CompStat::STATUS::SINGLE);
}

int Camera::Init()
{
	hdr = TextureManager::Create(owner->name + "camerascreen", SCREEN_W, SCREEN_H, DXGI_FORMAT_R16G16B16A16_FLOAT);
	depth = TextureManager::Create(owner->name + "camerascreendepth", SCREEN_W, SCREEN_H, DXGI_FORMAT_D32_FLOAT);

	constant_buffer_handle = CreateShaderConstantBuffer(sizeof(CBufferCameraInfo));

	gbuffer_texture_[0] = TextureManager::Create("gbuffer0", SCREEN_W, SCREEN_H, DXGI_FORMAT_R8G8B8A8_UNORM);
	gbuffer_texture_[1] = TextureManager::Create("gbuffer1", SCREEN_W, SCREEN_H, DXGI_FORMAT_R8G8B8A8_UNORM);
	gbuffer_texture_[2] = TextureManager::Create("gbuffer2", SCREEN_W, SCREEN_H, DXGI_FORMAT_R32G32B32A32_FLOAT);
	gbuffer_texture_[3] = TextureManager::Create("gbuffer3", SCREEN_W, SCREEN_H, DXGI_FORMAT_D32_FLOAT);

	if (!GetCurrentCamera())
		SetCurrentCamera();
	owner->GetScene()->RegisterActiveCamera(std::static_pointer_cast<Camera>(shared_from_this()));


	return 0;
}

void Camera::Draw()
{
	PrepareCamera();
	if (is_current_camera)
	{
		switch (render_type) {
		case RenderType::Forward:
		{
			SetRenderTarget(hdr.get(), depth.get());
			ClearColor(hdr.get(), { 0,0,0,0 });
			MV1SetPosition(sky_dome->GetHandle(), cast(owner->transform->position));
			ClearDepth(depth.get(), 1.0f);
			sky_dome->Draw(false);
			ClearDepth(depth.get(), 1.0f);
		}
		break;
		case RenderType::Deferred:
		{
			for (auto& gbuffer : gbuffer_texture_)
				ClearColor(gbuffer.get(), { 0,0,0,0 });
			MV1SetPosition(sky_dome->GetHandle(), cast(owner->transform->position));
			ClearDepth(gbuffer_texture_[GBUFFER_NUM - 1].get(), 1.0f);
			sky_dome->Draw(false);
			ClearDepth(gbuffer_texture_[GBUFFER_NUM - 1].get(), 1.0f);

		}
		}

	}
}

void Camera::LateDraw()
{
	if (is_current_camera) {
		if (render_type == RenderType::Deferred)
		{
			// GBuffer合成
			SetRenderTarget(nullptr, nullptr);
			auto context = GetD3DDeviceContext();
			// 深度バッファをコピー
			context->CopyResource(depth->D3dResource(), gbuffer_texture_[GBUFFER_NUM - 1]->D3dResource());

			SetRenderTarget(hdr.get(), depth.get());
			ClearColor(hdr.get(), { 0,0,0,0 });


			for (int i = 0; i < 16; i++)
				SetUseTextureToShader(i, -1);
			SetGbufferToSlot();
		}
	}

}

void Camera::PrepareCamera()
{
	auto owner_trns = owner->transform;
	if (is_current_camera)
	{
		SetCameraConstantBuffer();
		switch (render_type)
		{
		case Camera::RenderType::Forward:
		{
			SetRenderTarget(hdr.get(), depth.get());
		}
		break;
		case Camera::RenderType::Deferred:
		{
			std::array<Texture*, 3> gbuffer_texture = {
			gbuffer_texture_[0].get(),
			gbuffer_texture_[1].get(),
			gbuffer_texture_[2].get()
			};
			SetRenderTarget(GBUFFER_NUM - 1, gbuffer_texture.data(), gbuffer_texture_[GBUFFER_NUM - 1].get());

		}
		break;
		}
		SetupCamera_Perspective(DEG2RAD(perspective));
		SetCameraPositionAndTargetAndUpVec(cast(owner_trns->position), cast(owner_trns->position + owner_trns->AxisZ()), cast(owner_trns->AxisY()));
		SetCameraNearFar(camera_near, camera_far);
	}
}

void Camera::SetCameraConstantBuffer()
{
	if (!is_current_camera)
		return;
	void* p = GetBufferShaderConstantBuffer(constant_buffer_handle);
	if (!p)
		return;
	{
		auto owner_trns = owner->transform;
		auto* cb = reinterpret_cast<CBufferCameraInfo*>(p);
		cb->camera_pos = Vector4(owner_trns->position, 0);
		//cb->view = CreateMatrix::lookAtLH(owner_trns->position, owner_trns->position + owner_trns->AxisZ() * 10, Vector3(0, 1, 0));
		cb->view = mat4x4(
			Vector4(owner_trns->AxisX(), 0),
			Vector4(owner_trns->AxisY(), 0),
			Vector4(owner_trns->AxisZ(), 0),
			Vector4(owner_trns->position, 1)).inverseRT();
		cb->proj = CreateMatrix::perspectiveFovLH(DEG2RAD(perspective), (float)hdr->Width() / (float)hdr->Height(), camera_near, camera_far);
		cb->view_inv = cb->view.inverseRT();//CreateMatrix::lookAtLH(owner_trns->AxisZ(), owner_trns->position, Vector3(0, 1, 0));
		cb->proj_inv = CreateMatrix::InverseperspectiveFovLH(DEG2RAD(perspective), (float)hdr->Width() / (float)hdr->Height(), camera_near, camera_far);
		auto shadow_map = SceneManager::Object::Get<ShadowMapObject>(owner->GetScene());
		if (shadow_map) {
			auto light_view_projs = shadow_map->GetLightViewProjs();
			for (int i = 0; i < light_view_projs.size() && i < 4; i++)
				cb->mat_light_view_proj_[i] = light_view_projs[i];
		}

	}
	UpdateShaderConstantBuffer(constant_buffer_handle);
	SetShaderConstantBuffer(constant_buffer_handle, DX_SHADERTYPE_VERTEX, 10);
	SetShaderConstantBuffer(constant_buffer_handle, DX_SHADERTYPE_PIXEL, 10);
}


void Camera::SetCurrentCamera()
{
	if (GetCurrentCamera())
		GetCurrentCamera()->is_current_camera = false;
	if (!owner->GetScene())
		return;
	owner->GetScene()->SetCurrentCamera(std::static_pointer_cast<Camera>(shared_from_this()));
	is_current_camera = true;
	PrepareCamera();
}

void Camera::Exit() {
	hdr.reset();
	depth.reset();

	for (auto& gbuffer : gbuffer_texture_)
		gbuffer.reset();
	DeleteShaderConstantBuffer(constant_buffer_handle);
	owner->GetScene()->UnregisterActiveCamera(std::static_pointer_cast<Camera>(shared_from_this()));
	sky_dome.reset();
	sky_texture.reset();

}

void Camera::SetGbufferToSlot() const
{
	SetTexture(7, gbuffer_texture_[0].get());
	SetTexture(8, gbuffer_texture_[1].get());
	SetTexture(9, gbuffer_texture_[2].get());
	SetTexture(10, gbuffer_texture_[3].get());
}

SafeSharedPtr<Camera> Camera::GetCurrentCamera()
{
	if (!owner->GetScene())
		return nullptr;
	return SafeStaticCast<Camera>(owner->GetScene()->GetCurrentCamera().lock());
}


#if 0
void DebugCamera::Construct()
{
	if (auto another = owner->GetScene()->GetDebugCamera().lock())
	{
		if (another.raw_shared().get() != this) {
			RemoveThisComponent();
			return;
		}
	}
	SetPriority(1);
	status.status_bit.on(CompStat::STATUS::SINGLE);
}

int DebugCamera::Init()
{
	hdr = TextureManager::Create(owner->name + "d_camerascreen", SCREEN_W, SCREEN_H, DXGI_FORMAT_R8G8B8A8_UNORM);
	depth = TextureManager::Create(owner->name + "d_camerascreendepth", SCREEN_W, SCREEN_H, DXGI_FORMAT_D32_FLOAT);

	if (!gbuffer0)
		gbuffer0 = TextureManager::Create("gbuffer0", SCREEN_W, SCREEN_H, DXGI_FORMAT_R16G16_FLOAT);
	if (!gbuffer1)
		gbuffer1 = TextureManager::Create("gbuffer1", SCREEN_W, SCREEN_H, DXGI_FORMAT_R8G8B8A8_SNORM);
	if (!gbuffer2)
		gbuffer2 = TextureManager::Create("gbuffer2", SCREEN_W, SCREEN_H, DXGI_FORMAT_R11G11B10_FLOAT);


	return 0;
}

void DebugCamera::Update()
{
	if (Input::GetMouseButtonRepeat(MouseButton::ButtonRight)) {
		auto parent = owner->transform->parent.lock();
		auto owner_trns = owner->transform;
		if (Input::GetKey(KeyCode::W))
			parent->position += (owner_trns->AxisZ() * 10 * Time::RealDeltaTime());
		if (Input::GetKey(KeyCode::S))
			parent->position += (-owner_trns->AxisZ() * 10 * Time::RealDeltaTime());
		if (Input::GetKey(KeyCode::A))
			parent->position += (-owner_trns->AxisX() * 10 * Time::RealDeltaTime());
		if (Input::GetKey(KeyCode::D))
			parent->position += (owner_trns->AxisX() * 10 * Time::RealDeltaTime());
		if (Input::GetKey(KeyCode::Q))
			parent->position += (-owner_trns->AxisY() * 10 * Time::RealDeltaTime());
		if (Input::GetKey(KeyCode::E))
			parent->position += (owner_trns->AxisY() * 10 * Time::RealDeltaTime());
		Vector2 mouse_move = Input::GetMouseDelta();
		Quaternion qx = Quaternion(mouse_move.x * Time::RealDeltaTime() * 0.2f, { 0,1,0 });
		Quaternion qy = Quaternion(mouse_move.y * Time::RealDeltaTime() * 0.2f, { 1,0,0 });

		owner->transform->local_rotation = qy * owner->transform->local_rotation;
		parent->AddRotation(qx);
	}
}

void DebugCamera::PreDraw()
{
	PrepareCamera();
}


void DebugCamera::PrepareCamera()
{

	ClearColor(hdr.raw_shared().get(), { 0,0,0,0 });
	ClearDepth(depth.raw_shared().get(), 1.0f);
	TransformP owner_trns = owner->transform;
	std::array<Texture*, 4> gbuffers = { hdr.get(), gbuffer0.get(), gbuffer1.get(), gbuffer2.get() };
	SetRenderTarget(hdr.raw_shared().get(), depth.raw_shared().get());
	SetRenderTarget(4, gbuffers.data(), depth.get());

	SetupCamera_Perspective(DEG2RAD(perspective));
	SetCameraPositionAndTargetAndUpVec(cast(owner_trns->position), cast(owner_trns->position + owner_trns->AxisZ()), cast(owner_trns->AxisY()));
	SetCameraNearFar(camera_near, camera_far);
}

void DebugCamera::Exit()
{
	owner->GetScene()->SetDebugCamera(nullptr);
	hdr.reset();
	depth.reset();
	if (gbuffer0)
		gbuffer0.reset();
	if (gbuffer1)
		gbuffer1.reset();
	if (gbuffer2)
		gbuffer2.reset();
	auto targets = GetRenderTarget();
	//もし自分のレンダーターゲットがセットされていたらデフォルトに戻す
	if (targets.color_targets_[0]) {
		if (targets.color_targets_[0] == hdr.raw_shared().get())
		{
			auto current = GetCurrentCamera();
			SetRenderTarget(current->hdr.raw_shared().get(), current->depth.raw_shared().get());
		}
	}

}
#endif

void SetUpSkyboxResources(const SafeSharedPtr<Texture>& default_texture)
{

	ModelManager::LoadAsModel(u8"data/DebugBox/SkyDome.mv1", "sys__sky");
	if (!default_texture) {
		TextureManager::Load(u8"data/DebugBox/textures/night1.png", "sys__sky_texture");
		sky_texture = TextureManager::CloneByName("sys__sky_texture");
	}
	else {
		sky_texture = default_texture;
	}
	sky_dome = ModelManager::CloneModelByName("sys__sky");

	MV1SetTextureGraphHandle(sky_dome->GetHandle(), 0, *sky_texture, false);
	sky_dome->use_lighting = false;

}

void SetSkyboxTexture(const SafeSharedPtr<Texture>& texture)
{
	if (!texture)
		return;
	sky_texture = texture;
	MV1SetTextureGraphHandle(sky_dome->GetHandle(), 0, *sky_texture, false);
}

void ReleaseSkyboxResources()
{
	sky_dome.reset();
	sky_texture.reset();
}
