#include "precompile.h"
#include "SceneShader.h"
#include "System/Components/Camera.h"

VECTOR light_pos = { 5, 5, -5 };
VECTOR light_look = { 0, 0, 0 };
VECTOR light_up = { 0, 1, 0 };
int SceneShader::Init()
{
	model[0] = MV1LoadModel("data/model.mv1");
	model[1] = MV1LoadModel("data/Stage/stage00.mv1");
	auto variant = 8;
	shader_vs = std::make_shared<ShaderVs>("data/shader/vs_model.fx", variant);
	shader_ps = std::make_shared<ShaderPs>("data/shader/SimpleShader.fx");
	light_shader_vs = std::make_shared<ShaderVs>("data/shader/vs_shadow.fx", variant);
	light_shader_ps = std::make_shared<ShaderPs>("data/shader/ps_shadow.fx");
	shadow_map = MakeScreen(1024, 1024, false);
	Time::SetFPSMAX(100000000);
	MV1SetScale(model[0], VGet(0.02f, 0.02f, 0.02f));
	MV1SetScale(model[1], VGet(0.002f, 0.002f, 0.002f));
	obj = SceneManager::Object::Create<Object>();
	obj->transform->position = { 0,5,-5 };
	obj->transform->AddRotation({ 45,0,0 });
	camera = obj->AddComponent<Camera>();
	return 0;
}

void SceneShader::Update()
{
	Shader::updateFileWatcher();
	SetUseLighting(false);
	SetLightEnable(false);
}

void SceneShader::Draw()
{

#if 1
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetDrawScreen(shadow_map);
	ClearDrawScreen();
	SetCameraNearFar(0.1f, 3000.0f);
	SetDrawArea(0, 0, 1023, 1023);
	SetupCamera_Ortho(10);
	SetCameraPositionAndTarget_UpVecY(light_pos, light_look);
	//ユーザーカスタムシェーダーをセット
//頂点シェーダー
	SetUseVertexShader(*light_shader_vs);
	//ピクセルシェーダー
	SetUsePixelShader(*light_shader_ps);

	for (int i = 0; i < 2; i++)
		//フレーム(リグ)単位でメッシュを描画
		for (s32 frame_index = 0; frame_index < MV1GetFrameNum(model[i]); frame_index++)
		{
			//フレームのメッシュ数を取得
			s32 mesh_count = MV1GetFrameMeshNum(model[i], frame_index);
			for (s32 mesh_index = 0; mesh_index < mesh_count; mesh_index++)
			{

				//メッシュ番号を取得
				s32 mesh = MV1GetFrameMesh(model[i], frame_index, mesh_index);

				//メッシュのトライアングルリスト数を取得
				s32 tlist_cout = MV1GetMeshTListNum(model[i], mesh);
				for (s32 tlist_index = 0; tlist_index < tlist_cout; tlist_index++)
				{
					//トライアングルリスト番号を取得
					s32 tlist = MV1GetMeshTList(model[i], mesh, tlist_index);

					//トライアングルリストが使用しているマテリアルのインデックスを取得
					auto material_index = MV1GetTriangleListUseMaterial(model[i], tlist);
					//--------------------------------------------------
					// シェーダーバリエーションを選択
					//--------------------------------------------------
					// 頂点データタイプ(DX_MV1_VERTEX_TYPE_1FRAME 等)
					auto vertex_type = MV1GetTriangleListVertexType(model[i], tlist);
					u32  variant_vs = vertex_type;    // DXライブラリの頂点タイプをそのままバリエーション番号に

					//--------------------------------------------------
					// トライアングルリストを描画
					//--------------------------------------------------
					int handle_vs = light_shader_vs->variant(variant_vs);
					int handle_ps = *light_shader_ps;

					// シェーダーがない場合はオリジナルシェーダー利用を無効化
					bool shader_enable = (handle_vs != -1) && (handle_ps != -1);
					DxLib::MV1SetUseOrigShader(shader_enable);
					SetUseVertexShader(handle_vs);
					SetUsePixelShader(handle_ps);


					//トライアングルリストを描画
					MV1DrawTriangleList(model[i], tlist);
				}

				//メッシュごとに描画
				//MV1DrawMesh(model[i], mesh);
			}
		}
	MATRIX lightViewProjMatrix = MMult(GetCameraViewportMatrix(), GetCameraProjectionMatrix());
	SetVSConstFMtx(1, lightViewProjMatrix);

	//オリジナルシェーダーを使用するフラグをoff(後片付け)
	MV1SetUseOrigShader(false);

	if (Input::GetKeyDown(KeyCode::Space))
	{
		SaveDrawValidGraph(shadow_map, 0, 0, 1024, 1024, "data/shadow_map.png");
	}




	SetDrawScreen(DX_SCREEN_BACK);
	DrawFormatString(0, 0, Color::RED, "FPS:%.1f", Time::GetFPS());
	SetDrawArea(0, 0, SCREEN_W - 1, SCREEN_H - 1);

#endif
	camera->PreDraw();

	for (int i = -10; i <= 10; i++)
		for (int j = -10; j <= 10; j++) {
			DrawLine3D(VGet(i, 0, -10), VGet(i, 0, 10), Color::RED);
			DrawLine3D(VGet(-10, 0, j), VGet(10, 0, j), Color::RED);
		}

	if constexpr (false) {


		//ユーザーカスタムシェーダーをセット
		//頂点シェーダー
		SetUseVertexShader(*shader_vs);
		//ピクセルシェーダー
		SetUsePixelShader(*shader_ps);

		//オリジナルシェーダーを使用するフラグをon
		//MV1SetUseOrigShader(true);
		MV1DrawModel(model[0]);

		//オリジナルシェーダーを使用するフラグをoff(後片付け)
		MV1SetUseOrigShader(false);
		DrawExtendGraph(0, 0, 200, 200, shadow_map, false);
		return;
	}



	for (int i = 0; i < 2; i++)
		//フレーム(リグ)単位でメッシュを描画
		for (s32 frame_index = 0; frame_index < MV1GetFrameNum(model[i]); frame_index++)
		{
			//フレームのメッシュ数を取得
			s32 mesh_count = MV1GetFrameMeshNum(model[i], frame_index);
			for (s32 mesh_index = 0; mesh_index < mesh_count; mesh_index++)
			{

				//メッシュ番号を取得
				s32 mesh = MV1GetFrameMesh(model[i], frame_index, mesh_index);

				//メッシュのトライアングルリスト数を取得
				s32 tlist_cout = MV1GetMeshTListNum(model[i], mesh);
				for (s32 tlist_index = 0; tlist_index < tlist_cout; tlist_index++)
				{
					//トライアングルリスト番号を取得
					s32 tlist = MV1GetMeshTList(model[i], mesh, tlist_index);

					//トライアングルリストが使用しているマテリアルのインデックスを取得
					auto material_index = MV1GetTriangleListUseMaterial(model[i], tlist);
					//--------------------------------------------------
					// シェーダーバリエーションを選択
					//--------------------------------------------------
					// 頂点データタイプ(DX_MV1_VERTEX_TYPE_1FRAME 等)
					auto vertex_type = MV1GetTriangleListVertexType(model[i], tlist);
					u32  variant_vs = vertex_type;    // DXライブラリの頂点タイプをそのままバリエーション番号に

					//--------------------------------------------------
					// トライアングルリストを描画
					//--------------------------------------------------
					int handle_vs = shader_vs->variant(variant_vs);
					int handle_ps = *shader_ps;

					// シェーダーがない場合はオリジナルシェーダー利用を無効化
					bool shader_enable = (handle_vs != -1) && (handle_ps != -1);
					DxLib::MV1SetUseOrigShader(shader_enable);
					SetUseVertexShader(handle_vs);
					SetUsePixelShader(handle_ps);

					//トライアングルリストを描画
					MV1DrawTriangleList(model[i], tlist);
				}

				//メッシュごとに描画
				//MV1DrawMesh(model[i], mesh);
			}
		}

	//オリジナルシェーダーを使用するフラグをoff(後片付け)
	MV1SetUseOrigShader(false);
	DrawExtendGraph(0, 0, 200, 200, shadow_map, false);

}

void SceneShader::Exit()
{
	shader_ps.reset();
	shader_vs.reset();
	for (int i = 0; i < 2; i++)
		MV1DeleteModel(model[i]);
	DeleteGraph(shadow_map);
}
