//---------------------------------------------------------------------------
//! @file   ShadowMapObject.h
//! @brief  シャドウマップ（カスケードシャドウ）の描画を管理するオブジェクト
//---------------------------------------------------------------------------
#pragma once


//---------------------------------------------------------------------
//! @struct ShadowInfo
//! @brief  カスケードシャドウマップ用のカメラ視錐台・行列情報を保持する構造体
//---------------------------------------------------------------------
struct ShadowInfo {
	//       [4]+------ --+[5]
	//        ／|  　  ／ |
	//      ／  |6  ／    |
	// [0] +-----+ [1]   +[7]
	//     |     |   ／
	// [2] +-----+ [3]
	Vector3 frustum_vertices_[8];	//!< 視錐台の8頂点

	float  near_z_;					//!< ニアクリップ距離
	float  far_z_;					//!< ファークリップ距離
	float  aspect_ratio_;			//!< アスペクト比
	float  fov_y_;					//!< 垂直視野角（ラジアン）
	mat4x4 mat_camera_world_;		// シーンのカメラのワールド行列

	Vector4 bounding_sphere_;		// バウンディング球  [xyz_]:中心座標 [___w]:半径

public:
	//! @brief 視錐台情報を更新する
	void update();

	//! @brief デバッグ描画を行う
	void draw();
};

class ModelRenderer;

//---------------------------------------------------------------------
//! @class ShadowMapObject
//! @brief カスケードシャドウマップの生成・描画を担当するオブジェクトクラス
//---------------------------------------------------------------------
class ShadowMapObject :
	public Object
{
public:
	ShadowMapObject() :Object(1) {}
	USING_SUPER(ShadowMapObject);
	//! @brief 初期化処理
	int Init() override;
	//! @brief 描画前処理（シャドウマップの生成開始）
	void PreDraw() override;
	//! @brief 描画後処理（シャドウマップの生成終了）
	void LateDraw() override;
	//! @brief 終了処理
	void Exit() override;

	//! @brief シャドウマップのテクスチャサイズを設定する（初期化前のみ有効）
	inline void SetShadowMapSize(u32 size) {
		//初期化前にしか設定できない
		if (is_initialized)
			return;
		shadow_map_size = size;
	}

	//! @brief カスケード分割数を設定する（初期化前のみ有効）
	inline void SetCascadeCount(u32 count) {
		//初期化前にしか設定できない
		if (is_initialized)
			return;
		cascade_count = count;
	}

	//! @brief シャドウマップのテクスチャサイズを取得する
	inline u32 ShadowMapSize() const { return shadow_map_size; }
	//! @brief カスケード分割数を取得する
	inline u32 CascadeCount() const { return cascade_count; }
	//! @brief モデルレンダラーをシャドウキャスターとして登録する
	void RegisterModelRenderer(ModelRenderer* renderer);
	//! @brief モデルレンダラーのシャドウキャスター登録を解除する
	void UnregisterModelRenderer(ModelRenderer* renderer);
	//! @brief ライト方向を設定する
	void SetLightDirection(const Vector3& dir) { light_dir = dir.getNormalized(); }
	//! @brief ライトビュー×プロジェクション行列の配列を取得する
	std::vector<mat4x4> GetLightViewProjs() const { return shadowmap_view_projs; }

private:
	//! @brief シャドウマップの内部初期化
	bool InitializeShadowMap();
	//! @brief シャドウマップへの描画開始
	void ShadowMapDrawBegin();
	//! @brief シャドウマップへの描画終了
	void ShadowMapDrawEnd();
	bool is_initialized = false;						//!< 初期化済みフラグ
	SafeSharedPtr<Texture> shadow_map = nullptr;		//!< シャドウマップテクスチャ
	u32 shadow_map_size = 1024;							//!< シャドウマップの1辺のサイズ（ピクセル）
	u32 cascade_count = 4;								//!< カスケード分割数
	std::vector<ModelRenderer*> model_renderers;		//!< シャドウキャストするモデルレンダラーリスト

	Vector3 shadowmap_center = { 0,0,0 };				//!< シャドウマップの中心座標
	Vector3 light_dir = { 0,-1,0 };						//!< ライト方向ベクトル
	mat4x4 shadowmap_view;		//ライトビュー行列
	mat4x4 shadowmap_proj;		//ライトプロジェクション行列
	std::vector<mat4x4> shadowmap_view_projs; //ライトビュー×プロジェクション行列
	std::vector<ShadowInfo> shadow_infos;	//カスケードシャドウマップ用情報

	//シャドウマップ用サンプラー
	ID3D11SamplerState* shadow_sampler = nullptr;	//!< シャドウマップ用サンプラーステート
};
