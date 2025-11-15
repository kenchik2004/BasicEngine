//----------------------------------------------------------------------------
//!	@file	Model.cpp
//!	@brief	モデル関連の実装。ModelSource、Model、Animation クラスのメソッドを実装。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#include "Model.h"

using namespace physx;

//----------------------------------------------------
// @brief ModelSource の初期化。
//----------------------------------------------------
void ModelSource::Init()
{
	if (is_initialized)
		return; // 既に初期化済みなら何もしない
	int mat_count = MV1GetMaterialNum(handle);
	if (mat_count <= 0)
		mat_count = 1;
	materials.resize(mat_count); // マテリアル配列をリサイズ

	//DxLib内のテクスチャキャッシュからテクスチャを取得
	for (int i = 0; i < materials.size(); i++) {
		materials[i] = MaterialManager::CreateMaterial(name + "_Material" + std::to_string(i)); // マテリアルを作成
		int dif_index = MV1GetMaterialDifMapTexture(handle, i);
		int norm_index = MV1GetMaterialNormalMapTexture(handle, i);
		int spec_index = MV1GetMaterialSpcMapTexture(handle, i);
		if (dif_index >= 0) {
			int tex_handle = MV1GetTextureGraphHandle(handle, dif_index);
			auto mat_tex_diffuse = make_safe_shared<Texture>(tex_handle);
			materials[i]->SetTexture(mat_tex_diffuse, Material::TextureType::Diffuse); // ディフューズテクスチャを設定
		}
		if (norm_index >= 0) {
			int tex_handle = MV1GetTextureGraphHandle(handle, norm_index);
			auto mat_tex_norm = make_safe_shared<Texture>(tex_handle);
			materials[i]->SetTexture(mat_tex_norm, Material::TextureType::Normal); // ノーマルテクスチャを設定
		}
		if (spec_index >= 0) {
			int tex_handle = MV1GetTextureGraphHandle(handle, spec_index);
			auto mat_tex_spec = make_safe_shared<Texture>(tex_handle);
			materials[i]->SetTexture(mat_tex_spec, Material::TextureType::Specular); // スペキュラーテクスチャを設定
		}
	}
	is_initialized = true; // 初期化完了
}

//----------------------------------------------------
// @brief ポリゴンを取得する。
//----------------------------------------------------
MV1_REF_POLYGONLIST* ModelSource::GetPolygon()
{
	if (!is_initialized)
		Init(); // 初期化されていない場合は初期化

	if (!polygons_loaded) {
		MV1SetupReferenceMesh(handle, -1, false); // 参照メッシュを設定
		ref_poly_ = MV1GetReferenceMesh(handle, -1, false); // 参照メッシュを取得
	} return &ref_poly_;
}

//----------------------------------------------------
// @brief 凸メッシュを取得または作成する。
//----------------------------------------------------
physx::PxConvexMesh* ModelSource::GetOrCreateConvexMesh()
{
	if (!is_initialized)
		Init(); // 初期化されていない場合は初期化
	if (!convex_mesh) {
		PxCookingParams params(PhysicsManager::GetPhysicsInstance()->getTolerancesScale()); // Cooking パラメータを取得
#ifndef PACKAGE_BUILD
		if (handle < 0)
			throw(Exception("凸メッシュ作成に失敗しました:モデルデータが無効です。モデルパスが有効か確認してください。", DEFAULT_EXCEPTION_PARAM)); // 例外を投げる
#endif
		GetPolygon(); // ポリゴンを取得

		PxConvexMeshDesc  desc; // 凸メッシュ記述
		// 頂点データ取得
		std::vector<PxVec3> vertices;
		auto meshroot_mat = MV1GetFrameBaseLocalMatrix(handle, 0); // メッシュルート行列を取得
#if 1
		for (int i = 0; i < ref_poly_.VertexNum; i++) {
			VECTOR vertex = VTransform(ref_poly_.Vertexs[i].Position, meshroot_mat); // 頂点を変換
			Vector3 vert = Vector3(vertex.x, vertex.y, vertex.z);
			vertices.push_back(vert); // 頂点を追加
			ref_poly_.Vertexs[i].Position = vertex; // 位置を更新
		}
#endif
		// インデックスデータ取得
		std::vector<PxU32> indices;
		for (int i = 0; i < ref_poly_.PolygonNum; i++) {
			PxU32 idx0 = ref_poly_.Polygons[i].VIndex[0];
			PxU32 idx1 = ref_poly_.Polygons[i].VIndex[1];
			PxU32 idx2 = ref_poly_.Polygons[i].VIndex[2];
			indices.push_back(idx0); // インデックスを追加
			indices.push_back(idx1);
			indices.push_back(idx2);
		}
		//凸メッシュ作成
		desc.points.count = static_cast<PxU32>(vertices.size()); // 頂点数を設定
		desc.points.stride = sizeof(Vector3); // ストライドを設定
		desc.points.data = vertices.data(); // データポインタを設定
		desc.indices.count = ref_poly_.PolygonNum; // インデックス数を設定
		desc.indices.stride = sizeof(PxU32) * 3; // ストライドを設定
		desc.indices.data = indices.data(); // データポインタを設定
		desc.flags |= PxConvexFlag::eCOMPUTE_CONVEX; // フラグを設定
		desc.flags |= PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES;
		desc.flags |= PxConvexFlag::eFAST_INERTIA_COMPUTATION;

#ifndef PACKAGE_BUILD
		if (!desc.isValid())
			throw(Exception("ERROR!!DESC_INVALID", DEFAULT_EXCEPTION_PARAM)); // 例外を投げる
#endif
		// CookingしてConvexMeshを作成
		PxDefaultMemoryOutputStream write_buffer;
		bool status{ PxCookConvexMesh(params, desc, write_buffer) }; // Cooking 実行

#ifndef PACKAGE_BUILD
		if (!status)
			throw(Exception("ERROR!!STATUS_INVALID", DEFAULT_EXCEPTION_PARAM)); // 例外を投げる
#endif

		PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
		convex_mesh = PhysicsManager::GetPhysicsInstance()->createConvexMesh(read_buffer); // 凸メッシュを作成
		Time::ResetTime(); // 時間をリセット
#ifndef PACKAGE_BUILD
		if (!convex_mesh)
			throw(Exception("ERROR!!MESH_INVALID", DEFAULT_EXCEPTION_PARAM)); // 例外を投げる
#endif
	}
	return convex_mesh; // 凸メッシュを返す
}

//----------------------------------------------------
// @brief 三角メッシュを取得または作成する。
//----------------------------------------------------
physx::PxTriangleMesh* ModelSource::GetOrCreateTriangleMesh()
{
	if (!is_initialized)
		Init(); // 初期化されていない場合は初期化

	if (!triangle_mesh) {
		physx::PxCookingParams params(PhysicsManager::GetPhysicsInstance()->getTolerancesScale()); // Cooking パラメータを取得
		params.midphaseDesc = physx::PxMeshMidPhase::eBVH34; // ミッドフェーズを設定
		GetPolygon(); // ポリゴンを取得
		physx::PxTriangleMeshDesc desc; // 三角メッシュ記述
		auto meshroot_mat = MV1GetFrameBaseLocalMatrix(handle, 0); // メッシュルート行列を取得
		// 頂点データ取得
		std::vector<physx::PxVec3> vertices;
		for (int i = 0; i < ref_poly_.VertexNum; i++) {
			VECTOR vertex = VTransform(ref_poly_.Vertexs[i].Position, meshroot_mat); // 頂点を変換
			vertices.push_back(Vector3(vertex.x, vertex.y, vertex.z)); // 頂点を追加
			ref_poly_.Vertexs[i].Position = vertex; // 位置を更新
		}

		desc.points.count = static_cast<physx::PxU32>(vertices.size()); // 頂点数を設定
		desc.points.stride = sizeof(Vector3); // ストライドを設定
		desc.points.data = vertices.data(); // データポインタを設定

		// インデックスデータ取得
		std::vector<physx::PxU32> indices;
		for (int i = 0; i < ref_poly_.PolygonNum; i++) {
			physx::PxU32 idx0 = ref_poly_.Polygons[i].VIndex[0];
			physx::PxU32 idx1 = ref_poly_.Polygons[i].VIndex[1];
			physx::PxU32 idx2 = ref_poly_.Polygons[i].VIndex[2];

			indices.push_back(idx0); // インデックスを追加
			indices.push_back(idx1);
			indices.push_back(idx2);
		}

		desc.triangles.count = ref_poly_.PolygonNum; // 三角形数を設定
		desc.triangles.stride = sizeof(physx::PxU32) * 3; // ストライドを設定
		desc.triangles.data = indices.data(); // データポインタを設定
		desc.flags = physx::PxMeshFlags(); // フラグを設定

#ifndef PACKAGE_BUILD
		if (!desc.isValid())
			throw(Exception("ERROR!!DESC_INVALID", DEFAULT_EXCEPTION_PARAM)); // 例外を投げる
#endif

		physx::PxDefaultMemoryOutputStream write_buffer;
		bool status{ PxCookTriangleMesh(params,desc, write_buffer) }; // Cooking 実行
#ifndef PACKAGE_BUILD
		if (!status)
			throw(Exception("ERROR!!STATUS_INVALID", DEFAULT_EXCEPTION_PARAM)); // 例外を投げる
#endif

		physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
		triangle_mesh = PhysicsManager::GetPhysicsInstance()->createTriangleMesh(read_buffer); // 三角メッシュを作成
		Time::ResetTime(); // 時間をリセット
#ifndef PACKAGE_BUILD
		if (!triangle_mesh)
			throw(Exception("ERROR!!MESH_INVALID", DEFAULT_EXCEPTION_PARAM)); // 例外を投げる
#endif
	}
	return triangle_mesh; // 三角メッシュを返す
}

//----------------------------------------------------
// @brief 凸メッシュを取得する。
//----------------------------------------------------
physx::PxConvexMesh* Model::GetConvexMesh()
{
	return original->GetOrCreateConvexMesh(); // 元のソースから取得
}

//----------------------------------------------------
// @brief 三角メッシュを取得する。
//----------------------------------------------------
physx::PxTriangleMesh* Model::GetTriangleMesh()
{
	return original->GetOrCreateTriangleMesh(); // 元のソースから取得
}

//----------------------------------------------------
// @brief コンストラクタ。
//----------------------------------------------------
Model::Model()
{
	instance++; // インスタンス数をインクリメント
}

//----------------------------------------------------
// @brief モデルを描画する。
//----------------------------------------------------
void Model::Draw(bool to_gbuffer) {
	//マテリアルが指定されていない場合、通常の描画を行う
	if (materials.size() == 0 || !use_lighting) {
		SetUseLighting(false); // ライティングを無効化
		MV1DrawModel(handle); // モデルを描画
		SetUseLighting(true); // ライティングを有効化
		return;
	}

	ID3D11DeviceContext* context = reinterpret_cast<ID3D11DeviceContext*>(const_cast<void*>(GetUseDirect3D11DeviceContext())); // コンテキストを取得
	//SetUseTextureToShader(14, *ibl_diffuse.get());
	//SetUseTextureToShader(15, *ibl_specular.get());
		//フレーム(リグ)単位でメッシュを描画
	for (s32 frame_index = 0; frame_index < MV1GetFrameNum(handle); frame_index++) // フレームごとにループ
	{
		//フレームのメッシュ数を取得
		s32 mesh_count = MV1GetFrameMeshNum(handle, frame_index);
		for (s32 mesh_index = 0; mesh_index < mesh_count; mesh_index++) // メッシュごとにループ
		{
			//メッシュ番号を取得
			s32 mesh = MV1GetFrameMesh(handle, frame_index, mesh_index);
			if (to_gbuffer)	//Gbufferとアルファブレンドは相性が悪いので、ブレンドを無効化する
				MV1SetMeshDrawBlendMode(handle, mesh, DX_BLENDMODE_NOBLEND); // ブレンドモードを設定
			else
				MV1SetMeshDrawBlendMode(handle, mesh, DX_BLENDMODE_ALPHA);
			//メッシュのトライアングルリスト数を取得
			s32 tlist_cout = MV1GetMeshTListNum(handle, mesh);
			for (s32 tlist_index = 0; tlist_index < tlist_cout; tlist_index++) // トライアングルリストごとにループ
			{
				//トライアングルリスト番号を取得
				s32 tlist = MV1GetMeshTList(handle, mesh, tlist_index);

				//トライアングルリストが使用しているマテリアルのインデックスを取得
				auto material_index = MV1GetTriangleListUseMaterial(handle, tlist);


				//マテリアルが指定されている場合、マテリアルに応じたシェーダーで描画する
				Material* cur_material = nullptr;
				if (material_index < materials.size())
					cur_material = materials[material_index]; // マテリアルを取得
				else if (materials.size() > 0)
					cur_material = materials[materials.size() - 1]; // 最後のマテリアルを使用

				ShaderVs* shader_vs;
				ShaderPs* shader_ps;
				//マテリアルが指定されていない場合、デフォルトマテリアルを使用
				if (!cur_material)
					cur_material = MaterialManager::GetDefaultMat3D(); // デフォルトマテリアルを取得

				{
					shader_ps = cur_material->GetPixelShader(); // ピクセルシェーダーを取得
					if (to_gbuffer)
						shader_ps = MaterialManager::GetDefaultMatGBuffer()->GetPixelShader(); // GBuffer 用シェーダーを取得
					shader_vs = cur_material->GetVertexShader(); // 頂点シェーダーを取得
				}
				//--------------------------------------------------
				// シェーダーバリエーションを選択
				//--------------------------------------------------
				// 頂点データタイプ(DX_MV1_VERTEX_TYPE_1FRAME 等)
				auto vertex_type = MV1GetTriangleListVertexType(handle, tlist);
				u32  variant_vs = vertex_type;    // DXライブラリの頂点タイプをそのままバリエーション番号に

				//--------------------------------------------------
				// トライアングルリストを描画
				//--------------------------------------------------

				if (shader_ps && shader_vs) {

					int handle_vs = shader_vs->variant(variant_vs); // バリエーションを取得
					int handle_ps = *shader_ps;

					// シェーダーがない場合はオリジナルシェーダー利用を無効化
					bool enable_shader = (handle_vs != -1) && (handle_ps != -1);
					DxLib::MV1SetUseOrigShader(enable_shader); // オリジナルシェーダーの使用を設定
					SetUseVertexShader(handle_vs); // 頂点シェーダーを設定
					SetUsePixelShader(handle_ps); // ピクセルシェーダーを設定
					// マテリアルのテクスチャをセット
					for (u32 i = 0; i < static_cast<u32>(Material::TextureType::Max); i++) {
						auto texture = cur_material->GetTexture(static_cast<Material::TextureType>(i)); // テクスチャを取得
						auto texture_srv = cur_material->GetTexture(static_cast<Material::TextureType>(i))->Srv();
						if (texture) {
							SetUseTextureToShader(i, *texture.get()); // テクスチャを設定
						}
						else {
							SetUseTextureToShader(i, -1); // テクスチャを無効化
						}
					}

				}

				SetDrawMode(DX_DRAWMODE_BILINEAR); // 描画モードを設定
				//トライアングルリストを描画
				MV1DrawTriangleList(handle, tlist); // トライアングルリストを描画

			}

		}

		for (u32 i = 0; i < static_cast<u32>(Material::TextureType::Max); i++) {
			SetUseTextureToShader(i, -1); // テクスチャをリセット
		}
	}
	DxLib::MV1SetUseOrigShader(false); // オリジナルシェーダーを無効化
	DxLib::SetWriteZBuffer3D(true); // Zバッファを有効化
}

//----------------------------------------------------
// @brief アニメーションを更新する。
//----------------------------------------------------
void Animation::Update(float speed, bool check_call_backs)
{
	current_time += Time::RealDeltaTime() * 60 * speed; // 時間を更新
	if (!check_call_backs)
		return; // コールバックをチェックしない場合は終了
	for (auto& call_back : call_backs) { // コールバックごとにループ
		//コールバックを実行する条件
		//1.コールバックがまだ実行されていない
		//2.スピードによって再生・逆再生が変わるので、正なら2a、負なら2bの条件を見る
		//2a.現在のフレームが実行フレームより前か(順再生なので、指定フレームよりも大きい数字で実行)
		//2b.現在のフレームが実行フレームよりも後か(逆再生なので指定フレームより若い数字で実行)
		if (!call_back.is_executed && (speed > 0 ? (current_time >= call_back.ex_frame) : (current_time <= call_back.ex_frame))) // 条件をチェック
		{
			call_back.is_executed = true; // 実行済みに設定
			if (call_back.function)
				call_back.function(); // 関数を実行
		}
	}
}

//----------------------------------------------------
// @brief コールバックを初期化する。
//----------------------------------------------------
void Animation::InitCallBacks()
{
	for (auto& call_back : call_backs) { // コールバックごとにループ
		call_back.is_executed = false; // 実行済みをリセット
	}
}

//----------------------------------------------------
// @brief コールバックを設定する。
//----------------------------------------------------
void Animation::SetCallBack(std::function<void()>& call_back, float execute_frame, std::string_view method_name)
{
	std::string name(method_name);
	auto it = method_names.find(name); // メソッド名を検索
	AnimationCallBack anim_call_back;
	anim_call_back.function = std::move(call_back); // 関数を設定
	anim_call_back.ex_frame = execute_frame; // 実行フレームを設定
	anim_call_back.is_executed = false; // 実行済みをfalseに
	//すでに存在するメソッド名の場合、上書きする(大抵同じ名前で登録する場合は実行フレームの変更が目的だろう)
	if (it != method_names.end()) {
		call_backs[it->second - 1] = std::move(anim_call_back); // 上書き
		return;
	}
	call_backs.push_back(std::move(anim_call_back)); // 追加
	method_names[name] = call_backs.size(); // マップに登録
}

//----------------------------------------------------
// @brief コールバックをリセットする。
//----------------------------------------------------
void Animation::ResetCallBack(std::string_view method_name)
{
	std::string name(method_name);
	auto ite = method_names.find(name); // メソッド名を検索
	if (ite == method_names.end())
		return; // 見つからない場合は何もしない
	call_backs.erase(call_backs.begin() + ite->second - 1); // コールバックを削除
	method_names.erase(ite); // マップから削除
}
