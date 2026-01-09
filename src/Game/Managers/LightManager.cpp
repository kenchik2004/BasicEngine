#include "LightManager.h"

//PointLightのバウンディングボックスをカメラ描画領域に投影する関数
//PointLightにおいて、画面外にあるものは描画すると重くなってしまうので、
//rangeとpositionからスクリーン内に入っているかどうかを判定し、
//画面外にある場合は描画しないようにする処理に使用する
Vector4 CalculateBoundingBoxInScreen(const Vector3& light_pos, float range, const mat4x4& view_proj_matrix)
{
	Vector3 points[8] = {
		{ light_pos.x - range, light_pos.y - range, light_pos.z - range },
		{ light_pos.x + range, light_pos.y - range, light_pos.z - range },
		{ light_pos.x - range, light_pos.y + range, light_pos.z - range },
		{ light_pos.x + range, light_pos.y + range, light_pos.z - range },
		{ light_pos.x - range, light_pos.y - range, light_pos.z + range },
		{ light_pos.x + range, light_pos.y - range, light_pos.z + range },
		{ light_pos.x - range, light_pos.y + range, light_pos.z + range },
		{ light_pos.x + range, light_pos.y + range, light_pos.z + range },
	};
	float min_x = FLT_MAX;
	float min_y = FLT_MAX;
	float max_x = -FLT_MAX;
	float max_y = -FLT_MAX;
	bool any_point_in_front = false;
	for (const auto& point : points) {
		Vector4 clip_space_pos = view_proj_matrix.transform(Vector4(point, 1.0f));

		if (clip_space_pos.w <= 0.0f)
			continue;
		any_point_in_front = true;
		Vector2 ndc_space_pos = Vector2(clip_space_pos.x, clip_space_pos.y) / clip_space_pos.w;
		Vector2 screen_space_pos = Vector2((ndc_space_pos.x + 1.0f) * 0.5f * SCREEN_W, (1.0f - ndc_space_pos.y) * 0.5f * SCREEN_H);

		min_x = min(min_x, screen_space_pos.x);
		min_y = min(min_y, screen_space_pos.y);
		max_x = max(max_x, screen_space_pos.x);
		max_y = max(max_y, screen_space_pos.y);
	}
	if (!any_point_in_front) {
		// 全ての点がカメラの後ろにある場合、画面外とみなす
		return Vector4(1, 1, -1, -1);

	}
	// 画面外判定
	if ((max_x >= 0 && max_y >= 0) && (min_x <= SCREEN_W && min_y <= SCREEN_H))
		return Vector4(min_x, min_y, max_x, max_y);
	return Vector4(1, 1, -1, -1);
}




ShaderPs* shader_ssao = nullptr;
ShaderPs* gaussian = nullptr;
ShaderPs* nd_filter = nullptr;
ShaderPs* bloom_combine = nullptr;

int LightManager::Init() {
	name = "LightManager";
	lights_cbuffer_handle = CreateShaderConstantBuffer(sizeof(Vector4) + sizeof(LightInfo) * MAX_FORWARD_LIGHTS);
	light_blend_shader = MaterialManager::LoadPixelShader("data/shader/ps_light_finish.fx", "ps_light_finish");
	specular_accumulation_texture = make_safe_shared<Texture>(SCREEN_W, SCREEN_H, DXGI_FORMAT_R11G11B10_FLOAT);
	diffuse_accumulation_texture = make_safe_shared<Texture>(SCREEN_W, SCREEN_H, DXGI_FORMAT_R11G11B10_FLOAT);
	if (!shader_ssao)
		shader_ssao = MaterialManager::LoadPixelShader("data/shader/ps_ssao.fx", "ps_ssao");
	{
		if (!gaussian)
			gaussian = MaterialManager::LoadPixelShader("data/shader/ps_gaussian7x7.fx", "ps_gaussian7x7");
		if (!nd_filter)
			nd_filter = MaterialManager::LoadPixelShader("data/shader/ps_neutral_density.fx", "ps_nd");
		u32 w = SCREEN_W / 2;
		u32 h = SCREEN_H / 2;
		DXGI_FORMAT dxgi_format = DXGI_FORMAT_R11G11B10_FLOAT;
		bloom_work_texture = make_safe_shared<Texture>(w, h, dxgi_format);
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			w = max(1u, w >> 1);
			h = max(1u, h >> 1);
			bloom_reduction_textures[i].first = make_safe_shared<Texture>(w, h, dxgi_format);
			bloom_reduction_textures[i].second = make_safe_shared<Texture>(w, h, dxgi_format);
		}
		if (!bloom_combine)
			bloom_combine = MaterialManager::LoadPixelShader("data/shader/ps_bloom.fx", "ps_bloom");

	}
	return 0;
}

void LightManager::Draw()
{
	auto camera = GetScene()->GetCurrentCamera();
	if (!camera)
		return;
	if (camera->render_type != Camera::RenderType::Forward)
		return;
	// フォワードレンダリング用ライト情報のセット
	// ディファードレンダリングと違い、フォワードレンダリングでは
	// シェーダーに送れるライト数に制限があるため、最大数までしか送らない
	// この際の選択方法は以下の通り
	// 1.平行光源がある場合、優先的に送る->その際は、最初に見つけた平行光源を送る
	// 2.1のあとに遅れる個数に余裕がある場合は、点光源を送る
	// 3.点光源は、以下のルールを満たしたものを優先的に送る
	//		・バウンディングボックスがカメラ描画領域に入っているもの
	//		・カメラから近いもの<-未実装

	{
		// カメラ情報の取得
		auto camera = GetScene()->GetCurrentCamera();
		auto camera_obj = camera.lock()->owner.lock();
		if (camera) {
			mat4x4 view = CreateMatrix::lookAtLH(
				camera_obj->transform->position,
				camera_obj->transform->position + camera_obj->transform->AxisZ(),
				camera_obj->transform->AxisY()
			);
			mat4x4 proj = CreateMatrix::perspectiveFovLH(DEG2RAD(camera->perspective), ((float)SCREEN_W) / SCREEN_H, camera->camera_near, camera->camera_far);
			camera_view_proj = proj * view;
		}
	}
	std::array<LightBase*, MAX_FORWARD_LIGHTS> forward_lights = { nullptr };
	u8 set_light_count = 0;
	// 1.平行光源のセット
	for (auto& light : lights) {
		if (!light)
			continue;
		if (light->type == LightType::Directional && set_light_count < MAX_FORWARD_LIGHTS) {
			forward_lights[set_light_count] = light.get();
			set_light_count++;
			//最大数に達していたら終了
			if (set_light_count >= MAX_FORWARD_LIGHTS)
				break;
		}
	}
	//この時点で最大数に達していなければ、
	// 2.点光源のセット
	if (set_light_count < MAX_FORWARD_LIGHTS) {
		for (auto& light : lights) {
			if (!light)
				continue;
			//平行光源でないことを確認
			if (light->type != LightType::Directional && set_light_count < MAX_FORWARD_LIGHTS) {
				Vector4 screen_box = CalculateBoundingBoxInScreen(light->position, static_cast<PointLight*>(light.get())->range, camera_view_proj);
				//画面外にある場合はスキップ
				if (screen_box.x > screen_box.z || screen_box.y > screen_box.w) {
					continue;
				}
				//TODO:カメラから近いものを優先するロジックを追加する
				forward_lights[set_light_count] = light.get();
				set_light_count++;
				//最大数に達していたら終了
				if (set_light_count >= MAX_FORWARD_LIGHTS)
					break;
			}
		}
	}

	// シェーダーにセット
	set_light_count = min(set_light_count, MAX_FORWARD_LIGHTS);

	void* p = GetBufferShaderConstantBuffer(lights_cbuffer_handle);
	if (!p)
		return;
	// ついでに、セットするライト数もセットしておく
	*(reinterpret_cast<int*>(p)) = set_light_count;
	//4バイトのint,12バイトのパディングがあるので、16バイトずらす
	p = reinterpret_cast<Vector4*>(p) + 1;
	for (size_t i = 0; i < set_light_count; i++) {
		{
			auto* cb = reinterpret_cast<LightInfo*>(p);
			cb += i;
			auto* light = forward_lights[i];
			if (!light)
				break;
			// ライト情報のセット
			cb->type = static_cast<int>(light->type);
			cb->position = light->position;
			cb->color = Vector3(light->color.r, light->color.g, light->color.b);
			if (light->type == LightType::Directional)
				cb->direction = static_cast<DirectionalLight*>(light)->direction.getNormalized();
			if (light->type == LightType::Point)
				cb->range = static_cast<PointLight*>(light)->range;
		}
	}
	DxLib::UpdateShaderConstantBuffer(lights_cbuffer_handle);
	DxLib::SetShaderConstantBuffer(lights_cbuffer_handle, DX_SHADERTYPE_PIXEL, 11);
}

void LightManager::LateDraw()
{
	auto camera = GetScene()->GetCurrentCamera();
	if (!camera)
		return;
	if (camera->render_type != Camera::RenderType::Deferred)
		return;
	auto current_rt = GetRenderTarget();
	if constexpr (true) {
		//--------------------------
		//ここにSSAOを計算するコードを追加予定
		auto& ao_buffer = camera->gbuffer_texture_[0];
		//albedoとaoがセットされているテクスチャを引っぺがす
		SetTexture(7, nullptr);
		SetRenderTarget(ao_buffer.get());
		//--------------------------
		// AO計算をして、ao_bufferに書き込む
		//--------------------------

		//書き込む前に、albedoを書き換えないようにブレンドモードを変更
		DxLib::SetDrawCustomBlendMode(
			true,
			DX_BLEND_ONE,
			DX_BLEND_ONE,
			DX_BLENDOP_MIX,
			DX_BLEND_ONE,
			DX_BLEND_ONE,
			DX_BLENDOP_MIX,
			255

		);

		FillRenderTarget(*shader_ssao);
		SetRenderTarget(nullptr);
		//--------------------------
		SetTexture(7, ao_buffer.get());

		//--------------------------
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	ClearColor(diffuse_accumulation_texture.get(), Color::BLACK);
	ClearColor(specular_accumulation_texture.get(), Color::BLACK);
	std::array<Texture*, 2> rt_textures = { diffuse_accumulation_texture.get(), specular_accumulation_texture.get() };
	SetRenderTarget(2, rt_textures.data(), nullptr);
	// ライトの描画
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
	// ライトごとに描画

	{
		// カメラ情報の取得
		auto camera = GetScene()->GetCurrentCamera();
		auto camera_obj = camera.lock()->owner.lock();
		if (camera) {
			mat4x4 view = CreateMatrix::lookAtLH(
				camera_obj->transform->position,
				camera_obj->transform->position + camera_obj->transform->AxisZ(),
				camera_obj->transform->AxisY()
			);
			mat4x4 proj = CreateMatrix::perspectiveFovLH(DEG2RAD(camera->perspective), ((float)SCREEN_W) / SCREEN_H, camera->camera_near, camera->camera_far);
			camera_view_proj = proj * view;
		}
	}
	for (auto& light : lights) {
		light->DrawToAccumulationBuffer();
	}
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);



	// 元に戻す
	SetRenderTarget(current_rt);
	// 合成してカメラのHDRバッファに書き込む
	SetTexture(21, diffuse_accumulation_texture.get());
	SetTexture(22, specular_accumulation_texture.get());
	FillRenderTarget(*light_blend_shader);
	if constexpr (true) {
		// ブルーム処理

		CopyToRenderTarget(bloom_work_texture.get(), current_rt.color_targets_[0], *nd_filter);
		float inv_w = 1.0f / bloom_work_texture->Width();	//1ピクセル当たりのU幅
		float inv_h = 1.0f / bloom_work_texture->Height();	//1ピクセル当たりのV高さ
		int offset_radius = 1; //ガウシアンフィルタのオフセット半径
		Texture* upper_mip_tex = bloom_work_texture.get();
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			//1段上の階層テクスチャから縮小コピー
			auto& work = bloom_reduction_textures[i];
			CopyToRenderTarget(work.first.get(), upper_mip_tex);
			Vector2 offset = { 0.0f,inv_h * offset_radius };
			gaussian->SetValue("FilterInfo.pixel_offset", &offset);
			gaussian->AplyConstantBuffers();
			CopyToRenderTarget(work.second.get(), work.first.get(), *gaussian);

			offset = { inv_w * offset_radius,0.0f };
			gaussian->SetValue("FilterInfo.pixel_offset", &offset);
			gaussian->AplyConstantBuffers();
			CopyToRenderTarget(work.first.get(), work.second.get(), *gaussian);

			upper_mip_tex = work.first.get();
		}
		//ブルーム合成
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			SetTexture(30 + i, bloom_reduction_textures[i].first.get());
		}
		CopyToRenderTarget(current_rt.color_targets_[0], bloom_work_texture.get(), *bloom_combine);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			SetTexture(30 + i, nullptr);
		}
	}


	SetTexture(21, nullptr);
	SetTexture(22, nullptr);

}

void LightManager::LateDebugDraw()
{
	return;
	int canceled_light_num = 0;
	for (auto& light : lights) {
		if (!light)
			continue;
		if (light->type == LightType::Point)
		{
			auto point_light = static_cast<PointLight*>(light.get());
			Vector4 screen_box = CalculateBoundingBoxInScreen(light->position, point_light->range, camera_view_proj);

			if (screen_box.x > screen_box.z || screen_box.y > screen_box.w) {
				canceled_light_num++;
				continue;
			}
			DxLib::DrawBoxAA(
				screen_box.x,
				screen_box.y,
				screen_box.z,
				screen_box.w,
				Color::RED,
				FALSE
			);
			DrawSphere3D(
				cast(light->position),
				point_light->range > 1.0f ? 1.0f : point_light->range,
				16,
				Color::YELLOW,
				Color::YELLOW,
				FALSE);
			DrawSphere3D(
				cast(light->position),
				point_light->range,
				16,
				Color::GREEN,
				Color::GREEN,
				FALSE
			);
		}
		else
		{
			auto directional_light = static_cast<DirectionalLight*>(light.get());
			DrawLine3D(cast(directional_light->position), cast(directional_light->position + directional_light->direction), Color::YELLOW);
		}
	}
	printfDx("%d Lights Debug Draw Canceled: Out of Screen\n", canceled_light_num);

}

void LightManager::Exit()
{
	for (auto& light : lights) {
		light->my_manager.reset();
	}
	lights.clear();
	if (lights_cbuffer_handle != -1)
		DeleteShaderConstantBuffer(lights_cbuffer_handle);
	diffuse_accumulation_texture.reset();
	specular_accumulation_texture.reset();
	light_blend_shader = nullptr;

}

void LightBase::Init()
{
	cbuffer_handle = CreateShaderConstantBuffer(sizeof(LightInfo) + sizeof(Vector4));
	is_initialized = true;
	switch (type) {
	case LightType::Directional:
		accumulation_shader = MaterialManager::LoadPixelShader("data/shader/ps_light_directional.fx", "ps_light_directional");
		break;
	case LightType::Point:
		accumulation_shader = MaterialManager::LoadPixelShader("data/shader/ps_light_point.fx", "ps_light_point");
		break;
	}
}

void DirectionalLight::DrawToAccumulationBuffer()
{
	SetLightConstantBuffer();
	FillRenderTarget(*accumulation_shader);
}

void DirectionalLight::SetLightConstantBuffer()
{
	void* p = GetBufferShaderConstantBuffer(cbuffer_handle);
	if (!p)
		return;
	*(reinterpret_cast<int*>(p)) = 1;
	//LightInfo構造体の前にライト数を記録するintがあるので、パディング分ずらす
	p = reinterpret_cast<Vector4*>(p) + 1;
	{
		auto* cb = reinterpret_cast<LightInfo*>(p);
		cb->position = position;
		cb->color = Vector3(color.r, color.g, color.b);
		cb->direction = -direction.getNormalized();
		cb->range = 0.0f;

	}
	DxLib::UpdateShaderConstantBuffer(cbuffer_handle);
	DxLib::SetShaderConstantBuffer(cbuffer_handle, DX_SHADERTYPE_PIXEL, 11);
}

void PointLight::DrawToAccumulationBuffer()
{
	if (!my_manager)
		return;
	Vector4 screen_box = CalculateBoundingBoxInScreen(position, range, my_manager->GetCameraViewProj());

	if (screen_box.x > screen_box.z || screen_box.y > screen_box.w)
		return; //画面外にある場合は描画しない

	SetLightConstantBuffer();
	FillRenderTarget(*accumulation_shader);
}

void PointLight::SetLightConstantBuffer()
{
	void* p = GetBufferShaderConstantBuffer(cbuffer_handle);
	if (!p)
		return;
	*(reinterpret_cast<int*>(p)) = 1;
	//LightInfo構造体の前にライト数を記録するintがあるので、パディング分ずらす
	p = reinterpret_cast<Vector4*>(p) + 1;
	{
		auto* cb = reinterpret_cast<LightInfo*>(p);
		cb->position = position;
		cb->color = Vector3(color.r, color.g, color.b);
		cb->range = range;
		cb->direction = Vector3(0, 0, 0);
		cb->intensity = intensity;

	}
	DxLib::UpdateShaderConstantBuffer(cbuffer_handle);
	DxLib::SetShaderConstantBuffer(cbuffer_handle, DX_SHADERTYPE_PIXEL, 11);

}
