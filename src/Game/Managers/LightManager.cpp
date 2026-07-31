// LightManager.cpp
//---------------------------------------------------------------------------
//! @file   LightManager.cpp
//! @brief  LightManagerの実装。ゲーム内ライト管理を行う
//---------------------------------------------------------------------------
#include "LightManager.h"

//PointLightのバウンディングボックスをカメラ描画領域に投影する関数
//PointLightにおいて、画面外にあるものは描画すると重くなってしまうので、
//rangeとpositionからスクリーン内に入っているかどうかを判定し、
//画面外にある場合は描画しないようにする処理に使用する
/// @brief 3D空間の点光源の影響範囲を2Dスクリーンの矩形領域に変換する
/// @param light_pos 光源のワールド座標
/// @param range 光源の影響半径
/// @param view_proj_matrix カメラのビュープロジェクション行列
/// @return スクリーン座標系での最小X 最小Y 最大X 最大Y を格納したVector4 画面外の場合は破綻した値を返す
Vector4 CalculateBoundingBoxInScreen(const Vector3& light_pos, float range, const mat4x4& view_proj_matrix)
{
	// 影響範囲を覆う境界箱の8頂点を定義する
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
	// 最小値と最大値を初期化する
	float min_x = FLT_MAX;
	float min_y = FLT_MAX;
	float max_x = -FLT_MAX;
	float max_y = -FLT_MAX;
	// カメラ前方に頂点が存在するかどうかのフラグ
	bool any_point_in_front = false;
	for (const auto& point : points) {
		// 行列を利用してクリップ空間の座標に変換する
		Vector4 clip_space_pos = view_proj_matrix.transform(Vector4(point, 1.0f));

		// W値が0以下の場合はカメラの後方にあると判定しスキップする
		if (clip_space_pos.w <= 0.0f)
			continue;
		// 前方にある頂点が1つでも存在したことを記録する
		any_point_in_front = true;
		// W除算を行い正規化デバイス座標系に変換する
		Vector2 ndc_space_pos = Vector2(clip_space_pos.x, clip_space_pos.y) / clip_space_pos.w;
		// ピクセル単位のスクリーン座標系に変換する
		Vector2 screen_space_pos = Vector2((ndc_space_pos.x + 1.0f) * 0.5f * SCREEN_W, (1.0f - ndc_space_pos.y) * 0.5f * SCREEN_H);

		// 矩形領域の最小値と最大値を更新する
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
	// 完全に画面外にある場合は固定の破綻した(min_x > max_x)値を返す
	return Vector4(1, 1, -1, -1);
}




ShaderPs* shader_ssao = nullptr;		//SSAO用シェーダー
ShaderPs* gaussian = nullptr;			//ガウシアンブラー用シェーダー
ShaderPs* nd_filter = nullptr;			//NDフィルター用シェーダー
ShaderPs* bloom_combine = nullptr;		//ブルーム合成用シェーダー
ShaderPs* tone_mapping = nullptr;		//トーンマッピング用シェーダー

/// @brief ライトマネージャーの初期化処理を行う
/// @return 初期化成功時は0を返す
int LightManager::Init() {
	name = "LightManager";
	// ライト情報の定数バッファを作成する
	lights_cbuffer_handle = CreateShaderConstantBuffer(sizeof(Vector4) + sizeof(LightInfo) * MAX_FORWARD_LIGHTS);
	// ライト合成用のピクセルシェーダーを読み込む
	light_blend_shader = MaterialManager::LoadPixelShader("data/shader/ps_light_finish.fx", "ps_light_finish");
	// スペキュラとディフューズの蓄積用テクスチャを生成する
	specular_accumulation_texture = make_safe_shared<Texture>(SCREEN_W, SCREEN_H, DXGI_FORMAT_R11G11B10_FLOAT);
	diffuse_accumulation_texture = make_safe_shared<Texture>(SCREEN_W, SCREEN_H, DXGI_FORMAT_R11G11B10_FLOAT);

	// SDR出力用のテクスチャを生成する
	sdr = make_safe_shared<Texture>(SCREEN_W, SCREEN_H, DXGI_FORMAT_R8G8B8A8_UNORM);

	// SSAO用のシェーダーが未読み込みであれば読み込む
	if (!shader_ssao)
		shader_ssao = MaterialManager::LoadPixelShader("data/shader/ps_ssao.fx", "ps_ssao");
	{
		// ポストエフェクト用の各種シェーダーを読み込む
		if (!gaussian)
			gaussian = MaterialManager::LoadPixelShader("data/shader/ps_gaussian7x7.fx", "ps_gaussian7x7");
		if (!nd_filter)
			nd_filter = MaterialManager::LoadPixelShader("data/shader/ps_neutral_density.fx", "ps_nd");
		// ブルーム処理用の初期解像度を決定する
		u32 w = SCREEN_W / 2;
		u32 h = SCREEN_H / 2;
		DXGI_FORMAT dxgi_format = DXGI_FORMAT_R11G11B10_FLOAT;
		// ブルーム作業用テクスチャを生成する
		bloom_work_texture = make_safe_shared<Texture>(w, h, dxgi_format);
		// 段階的に縮小したテクスチャペアを生成する
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			w = max(1u, w >> 1);	//bitシフトで1/2解像度にする
			h = max(1u, h >> 1);	//bitシフトで1/2解像度にする
			bloom_reduction_textures[i].first = make_safe_shared<Texture>(w, h, dxgi_format);
			bloom_reduction_textures[i].second = make_safe_shared<Texture>(w, h, dxgi_format);
		}
		// ブルーム合成とトーンマッピングのシェーダーを読み込む
		if (!bloom_combine)
			bloom_combine = MaterialManager::LoadPixelShader("data/shader/ps_bloom.fx", "ps_bloom");
		if (!tone_mapping)
			tone_mapping = MaterialManager::LoadPixelShader("data/shader/ps_tonemapping.fx", "ps_tonemapping");
	}
	return 0;
}

/// @brief フォワードレンダリング向けのライト描画準備を行う
/// @details シェーダーに渡すライトを選別して定数バッファを更新する
void LightManager::Draw()
{
	// 現在アクティブなカメラを取得する
	auto camera = GetScene()->GetCurrentCamera();
	// カメラが存在しなければ処理を中断する
	if (!camera)
		return;
	// レンダリングタイプがフォワードでなければ処理を中断する
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
			// カメラのビュー行列を計算する
			mat4x4 view = CreateMatrix::lookAtLH(
				camera_obj->transform->position,
				camera_obj->transform->position + camera_obj->transform->AxisZ(),
				camera_obj->transform->AxisY()
			);
			// カメラのプロジェクション行列を計算する
			mat4x4 proj = CreateMatrix::perspectiveFovLH(DEG2RAD(camera->perspective), ((float)SCREEN_W) / SCREEN_H, camera->camera_near, camera->camera_far);
			// ビュープロジェクション行列を合成して保持する
			camera_view_proj = proj * view;
		}
	}
	// フォワードレンダリング用のライト配列を空で初期化する
	std::array<LightBase*, MAX_FORWARD_LIGHTS> forward_lights = { nullptr };
	// シェーダーにセットしたライトの数を記録する変数
	u8 set_light_count = 0;
	// まず平行光源のセット(これは最優先)
	for (auto& light : lights) {
		if (!light)
			continue;
		// 平行光源でありかつ最大数に達していない場合は配列に追加する
		if (light->type == LightType::Directional && set_light_count < MAX_FORWARD_LIGHTS) {
			forward_lights[set_light_count] = light.get();
			set_light_count++;
			//最大数に達していたら終了
			if (set_light_count >= MAX_FORWARD_LIGHTS)
				break;
		}
	}
	//この時点で最大数に達していなければ、
	// 点光源のセット(余裕があれば使う)
	if (set_light_count < MAX_FORWARD_LIGHTS) {
		for (auto& light : lights) {
			if (!light)
				continue;
			//平行光源でないことを確認
			if (light->type != LightType::Directional && set_light_count < MAX_FORWARD_LIGHTS) {
				// スクリーン空間での影響範囲を計算する
				Vector4 screen_box = CalculateBoundingBoxInScreen(light->position, static_cast<PointLight*>(light.get())->range, camera_view_proj);
				//画面外にある場合はスキップ
				if (screen_box.x > screen_box.z || screen_box.y > screen_box.w) {
					continue;
				}
				//TODO:カメラから近いものを優先するロジックを追加する
				// 点光源を配列に追加してカウントを増やす
				forward_lights[set_light_count] = light.get();
				set_light_count++;
				//最大数に達していたら終了
				if (set_light_count >= MAX_FORWARD_LIGHTS)
					break;
			}
		}
	}

	// シェーダーにセット
	// セットするライト数が最大値を超えないように制限する
	set_light_count = min(set_light_count, MAX_FORWARD_LIGHTS);

	// 定数バッファのポインタを取得する
	void* p = GetBufferShaderConstantBuffer(lights_cbuffer_handle);
	if (!p)
		return;
	// ついでに、セットするライト数もセットしておく
	*(reinterpret_cast<int*>(p)) = set_light_count;
	//4バイトのint,12バイトのパディングがあるので、16バイトずらす
	p = reinterpret_cast<Vector4*>(p) + 1;
	// 収集したライトの情報を順番にバッファへ書き込む
	for (size_t i = 0; i < set_light_count; i++) {
		{
			// 書き込み先のアドレスを計算する
			auto* cb = reinterpret_cast<LightInfo*>(p);
			cb += i;
			auto* light = forward_lights[i];
			if (!light)
				break;
			// ライト情報のセット
			cb->type = static_cast<int>(light->type);
			cb->position = light->position;
			cb->color = Vector3(light->color.r, light->color.g, light->color.b);
			// 光源の種類に応じて固有のパラメータを設定する
			if (light->type == LightType::Directional)
				cb->direction = static_cast<DirectionalLight*>(light)->direction.getNormalized();
			if (light->type == LightType::Point)
				cb->range = static_cast<PointLight*>(light)->range;
		}
	}
	// 定数バッファをVRAMに転送しピクセルシェーダーに設定する
	DxLib::UpdateShaderConstantBuffer(lights_cbuffer_handle);
	DxLib::SetShaderConstantBuffer(lights_cbuffer_handle, DX_SHADERTYPE_PIXEL, 11);
}

/// @brief ディファードレンダリングにおける遅延描画処理を行う
/// @details Gバッファを利用してライティングやポストエフェクトを適用する
void LightManager::LateDraw()
{
	// 現在のカメラを取得する
	auto camera = GetScene()->GetCurrentCamera();
	// カメラが存在しなければ処理を中断する
	if (!camera)
		return;
	// 現在のレンダーターゲット状態を保存する
	auto current_rt = GetRenderTarget();
	// ディファードレンダリング対象のみ、ライト合成処理を行う
	if (camera->render_type == Camera::RenderType::Deferred)
	{
		if constexpr (true) {
			//--------------------------
			//ここにSSAOを計算するコードを追加予定
			// SSAOを書き込むバッファを取得する
			auto& ao_buffer = camera->gbuffer_texture_[0];
			//albedoとaoがセットされているテクスチャを引っぺがす
			SetTexture(7, nullptr);
			// レンダーターゲットをAOバッファに変更する
			SetRenderTarget(ao_buffer.get());
			//--------------------------
			// AO計算をして、ao_bufferに書き込む
			//--------------------------

			//書き込む前に、albedoを書き換えないようにブレンドモードを変更
			DxLib::SetDrawCustomBlendMode(
				true,
				DX_BLEND_ONE,
				DX_BLEND_ONE,
				DX_BLENDOP_MIX,		//MIXと書いてあるが、DxLibの誤字で、実際はMIN
				DX_BLEND_ONE,
				DX_BLEND_ONE,
				DX_BLENDOP_MIX,		//MIXと書いてあるが、DxLibの誤字で、実際はMIN
				255

			);

			// SSAOシェーダーを適用して描画する
			FillRenderTarget(*shader_ssao);
			// レンダーターゲットの設定を解除する
			SetRenderTarget(nullptr);
			//--------------------------
			// 描画結果をテクスチャとして再設定する
			SetTexture(7, ao_buffer.get());

			//--------------------------
			// ブレンドモードを通常に戻す
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		// 蓄積バッファを黒でクリアする
		ClearColor(diffuse_accumulation_texture.get(), Color::BLACK);
		ClearColor(specular_accumulation_texture.get(), Color::BLACK);
		// 2つの蓄積バッファを同時にレンダーターゲットとして設定する
		std::array<Texture*, 2> rt_textures = { diffuse_accumulation_texture.get(), specular_accumulation_texture.get() };
		SetRenderTarget(2, rt_textures.data(), nullptr);
		// ライトの描画
		// 光を加算合成するためにブレンドモードを変更する
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		// ライトごとに描画

		{
			// カメラ情報の取得
			auto camera = GetScene()->GetCurrentCamera();
			auto camera_obj = camera.lock()->owner.lock();
			if (camera) {
				// ビュー行列を再計算する
				mat4x4 view = CreateMatrix::lookAtLH(
					camera_obj->transform->position,
					camera_obj->transform->position + camera_obj->transform->AxisZ(),
					camera_obj->transform->AxisY()
				);
				// プロジェクション行列を再計算する
				mat4x4 proj = CreateMatrix::perspectiveFovLH(DEG2RAD(camera->perspective), ((float)SCREEN_W) / SCREEN_H, camera->camera_near, camera->camera_far);
				// ビュープロジェクション行列を更新する
				camera_view_proj = proj * view;
			}
		}
		// 登録されている全てのライトについて蓄積バッファへの描画処理を呼ぶ
		for (auto& light : lights) {
			light->DrawToAccumulationBuffer();
		}
		// ブレンドモードを通常に戻す
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);



		// 元に戻す
		// レンダーターゲットを保存していた状態に復元する
		SetRenderTarget(current_rt);
		// 合成してカメラのHDRバッファに書き込む
		// 蓄積されたライティング結果をテクスチャとして設定する
		SetTexture(21, diffuse_accumulation_texture.get());
		SetTexture(22, specular_accumulation_texture.get());
		// 合成用シェーダーを適用して最終的なライティング結果を描画する
		FillRenderTarget(*light_blend_shader);
	}
	if constexpr (true) {
		// ブルーム処理

		// 輝度抽出フィルタをかけて作業用テクスチャにコピーする
		CopyToRenderTarget(bloom_work_texture.get(), current_rt.color_targets_[0], *nd_filter);
		float inv_w = 1.0f / bloom_work_texture->Width();	//1ピクセル当たりのU幅
		float inv_h = 1.0f / bloom_work_texture->Height();	//1ピクセル当たりのV高さ
		int offset_radius = 2.0f; //ガウシアンフィルタのオフセット半径
		// コピー元のテクスチャポインタを初期化する
		Texture* upper_mip_tex = bloom_work_texture.get();
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			//1段上の階層テクスチャから縮小コピー
			auto& work = bloom_reduction_textures[i];
			// そのまま縮小してコピーする
			CopyToRenderTarget(work.first.get(), upper_mip_tex);
			// 垂直方向のガウスぼかしを適用する
			Vector2 offset = { 0.0f,inv_h * offset_radius };
			gaussian->SetValue("FilterInfo.pixel_offset", &offset);
			gaussian->AplyConstantBuffers();
			CopyToRenderTarget(work.second.get(), work.first.get(), *gaussian);

			// 水平方向のガウスぼかしを適用する
			offset = { inv_w * offset_radius,0.0f };
			gaussian->SetValue("FilterInfo.pixel_offset", &offset);
			gaussian->AplyConstantBuffers();
			CopyToRenderTarget(work.first.get(), work.second.get(), *gaussian);

			// 次のループのためにテクスチャポインタを更新する
			upper_mip_tex = work.first.get();
		}
		//ブルーム合成
		// ぼかした各解像度のテクスチャを加算合成する
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			SetTexture(30 + i, bloom_reduction_textures[i].first.get());
		}
		// 合成シェーダーを実行して元のターゲットにブルームを加える
		CopyToRenderTarget(current_rt.color_targets_[0], bloom_work_texture.get(), *bloom_combine);
		// ブレンドモードとテクスチャの設定を元に戻す
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		for (u32 i = 0; i < REDUCTION_COUNT_MAX; i++) {
			SetTexture(30 + i, nullptr);
		}
	}
	//トーンマッピング
	// HDRの色空間をSDRに変換してSDRバッファに書き込む
	CopyToRenderTarget(sdr.get(), current_rt.color_targets_[0], *tone_mapping);
	// トーンマッピング結果を元のターゲットに書き戻す
	CopyToRenderTarget(current_rt.color_targets_[0], sdr.get());


	// テクスチャの設定を解除する
	SetTexture(21, nullptr);
	SetTexture(22, nullptr);

}

/// @brief ライト領域のデバッグ描画処理を行う
/// @details 点光源の影響範囲や平行光源の方向を可視化する
void LightManager::LateDebugDraw()
{
	return;
	// 描画をキャンセルしたライトの数を数える変数を初期化する
	int canceled_light_num = 0;
	// 登録されている全てのライトについてデバッグ描画を試みる
	for (auto& light : lights) {
		// 無効なポインタはスキップする
		if (!light)
			continue;
		if (light->type == LightType::Point)
		{
			// 点光源の場合は影響範囲の境界をスクリーン空間で計算する
			auto point_light = static_cast<PointLight*>(light.get());
			Vector4 screen_box = CalculateBoundingBoxInScreen(light->position, point_light->range, camera_view_proj);

			// 完全に画面外にある場合はカウントを増やしてスキップする
			if (screen_box.x > screen_box.z || screen_box.y > screen_box.w) {
				canceled_light_num++;
				continue;
			}
			// スクリーン上のバウンディングボックスを赤い矩形で描画する
			DxLib::DrawBoxAA(
				screen_box.x,
				screen_box.y,
				screen_box.z,
				screen_box.w,
				Color::RED,
				FALSE,
				2.0f
			);
			// 影響範囲の中心付近に黄色の小さな球を描画する
			DrawSphere3D(
				cast(light->position),
				point_light->range > 1.0f ? 1.0f : point_light->range,
				16,
				Color::YELLOW,
				Color::YELLOW,
				FALSE);
			// 影響範囲全体を示す緑色の球を描画する
			DrawSphere3D(
				cast(light->position),
				point_light->range,
				8,
				Color::GREEN,
				Color::GREEN,
				FALSE
			);
		}
		else
		{
			// 平行光源の場合は照射方向を示す黄色い線を描画する
			auto directional_light = static_cast<DirectionalLight*>(light.get());
			DrawLine3D(cast(directional_light->position), cast(directional_light->position + directional_light->direction), Color::YELLOW);
		}
	}
	// スキップされたライトの数をコンソールに出力する
	printfDx("%d Lights Debug Draw Canceled: Out of Screen\n", canceled_light_num);

}

/// @brief ライトマネージャーの終了処理を行う
/// @details 確保したリソースや登録されたライトを全て解放する
void LightManager::Exit()
{
	// 各ライトが持つマネージャーへの参照をリセットする
	for (auto& light : lights) {
		light->my_manager.reset();
	}
	// ライトの管理リストを空にする
	lights.clear();
	// 定数バッファが有効であれば削除する
	if (lights_cbuffer_handle != -1)
		DeleteShaderConstantBuffer(lights_cbuffer_handle);
	// 確保したテクスチャのスマートポインタをリセットする
	diffuse_accumulation_texture.reset();
	specular_accumulation_texture.reset();
	// シェーダーのポインタをクリアする
	light_blend_shader = nullptr;

}

/// @brief ライト基底クラスの初期化処理を行う
/// @details 定数バッファの作成と種類に応じたシェーダーの読み込みを実施する
void LightBase::Init()
{
	// ライト情報用の定数バッファを作成する
	cbuffer_handle = CreateShaderConstantBuffer(sizeof(LightInfo) + sizeof(Vector4));
	// 初期化済みフラグを立てる
	is_initialized = true;
	// ライトの種類に応じて対応するピクセルシェーダーを読み込む
	switch (type) {
	case LightType::Directional:
		accumulation_shader = MaterialManager::LoadPixelShader("data/shader/ps_light_directional.fx", "ps_light_directional");
		break;
	case LightType::Point:
		accumulation_shader = MaterialManager::LoadPixelShader("data/shader/ps_light_point.fx", "ps_light_point");
		break;
	}
}

/// @brief 平行光源を蓄積バッファに描画する
void DirectionalLight::DrawToAccumulationBuffer()
{
	// 定数バッファに現在のパラメータを設定する
	SetLightConstantBuffer();
	// 全画面に対してシェーダーを実行しライティングを蓄積する
	FillRenderTarget(*accumulation_shader);
}

/// @brief 平行光源の情報を定数バッファに書き込む
void DirectionalLight::SetLightConstantBuffer()
{
	// 定数バッファのポインタを取得する
	void* p = GetBufferShaderConstantBuffer(cbuffer_handle);
	if (!p)
		return;
	// ライトの数を1に設定する
	*(reinterpret_cast<int*>(p)) = 1;
	//LightInfo構造体の前にライト数を記録するintがあるので、パディング分ずらす
	p = reinterpret_cast<Vector4*>(p) + 1;
	{
		// 構造体の各メンバに平行光源の情報を書き込む
		auto* cb = reinterpret_cast<LightInfo*>(p);
		cb->position = position;
		cb->color = Vector3(color.r, color.g, color.b);
		cb->direction = -direction.getNormalized();
		cb->range = 0.0f;

	}
	// 定数バッファをVRAMに転送しシェーダーに設定する
	DxLib::UpdateShaderConstantBuffer(cbuffer_handle);
	DxLib::SetShaderConstantBuffer(cbuffer_handle, DX_SHADERTYPE_PIXEL, 11);
}

/// @brief 点光源を蓄積バッファに描画する
/// @details 画面内に存在する場合のみ描画コマンドを発行する
void PointLight::DrawToAccumulationBuffer()
{
	// マネージャーが登録されていなければ処理を中断する
	if (!my_manager)
		return;
	// スクリーン座標系での影響範囲バウンディングボックスを計算する
	Vector4 screen_box = CalculateBoundingBoxInScreen(position, range, my_manager->GetCameraViewProj());

	// 完全に画面外にある場合は描画処理をスキップする
	if ((screen_box.x > screen_box.z || screen_box.y > screen_box.w))
		return; //画面外にある場合は描画しない

	// 定数バッファに現在のパラメータを設定する
	SetLightConstantBuffer();
	// 全画面に対してシェーダーを実行しライティングを蓄積する
	FillRenderTarget(*accumulation_shader);
}

/// @brief 点光源の情報を定数バッファに書き込む
void PointLight::SetLightConstantBuffer()
{
	// 定数バッファのポインタを取得する
	void* p = GetBufferShaderConstantBuffer(cbuffer_handle);
	if (!p)
		return;
	// ライトの数を1に設定する
	*(reinterpret_cast<int*>(p)) = 1;
	//LightInfo構造体の前にライト数を記録するintがあるので、パディング分ずらす
	p = reinterpret_cast<Vector4*>(p) + 1;
	{
		// 構造体の各メンバに点光源の情報を書き込む
		auto* cb = reinterpret_cast<LightInfo*>(p);
		cb->position = position;
		cb->color = Vector3(color.r, color.g, color.b);
		cb->range = range;
		cb->direction = Vector3(0, 0, 0);
		cb->intensity = intensity;

	}
	// 定数バッファをVRAMに転送しシェーダーに設定する
	DxLib::UpdateShaderConstantBuffer(cbuffer_handle);
	DxLib::SetShaderConstantBuffer(cbuffer_handle, DX_SHADERTYPE_PIXEL, 11);

}

