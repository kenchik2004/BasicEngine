//---------------------------------------------------------------------------
//! @file   LightManager.h
//! @brief  ライトマネージャー（ゲームのライト管理）
//---------------------------------------------------------------------------
#pragma once



//TODO: ライトの管理をオブジェクトではなくマネージャ作成+コンポーネント化に変更する。
//構成案：
// LightManager(シーンごとに持たせるシステムのマネージャ)
//		コンポーネントの持っているLight構造体vectorを持つ
//   ├ LightComponent(各オブジェクトに持たせるコンポーネント)
//      ├Light構造体(ライトの情報を持つ構造体、LightBase継承)
enum class LightType :u32 {
	Directional,
	Point,
};
struct LightInfo {
	Vector3 position; //!< 光源の位置座標
	float range; //!< 光源の影響範囲
	Vector3 direction; //!< 光源の向き
	int type;			//!<ライトタイプ
	Vector3 color; //!< 光源の色
	float intensity;	//!< 減衰率

};
//---------------------------------------------------------------------
//! @class LightBase
//! @brief ライトの基底クラス
//---------------------------------------------------------------------
class LightBase {
	friend class LightManager;
public:
	virtual ~LightBase() {}

	//! @brief コンストラクタ
	LightBase(const Vector3& pos, const Color& col, const LightType& l_type) : position(pos), color(col), type(l_type) {}
	//! @brief 初期化処理
	void Init();
	LightType type; //!< ライトの種類
	Vector3 position; //!< ライトの位置
	Color color; //!< ライトの色
protected:
	SafeWeakPtr<LightManager> my_manager = nullptr; //!< 所属するライトマネージャー
	ShaderPs* accumulation_shader = nullptr; //!< 蓄積バッファ描画用シェーダー
	bool is_initialized = false; //!< 初期化済みフラグ
	int cbuffer_handle = -1; //!< 定数バッファハンドル
	//! @brief 蓄積バッファへの描画
	virtual void DrawToAccumulationBuffer() = 0;
	//! @brief ライト定数バッファの設定
	virtual void SetLightConstantBuffer() = 0;
};

//---------------------------------------------------------------------
//! @class DirectionalLight
//! @brief 平行光源クラス
//---------------------------------------------------------------------
class DirectionalLight :public LightBase
{
public:
	Vector3 direction; //!< ライトの照射方向
	DirectionalLight(const Vector3& pos, const Color& col, const Vector3& dir)
		: LightBase(pos, col, LightType::Directional) {
		direction = dir.getNormalized();
	}
protected:
	//! @brief 蓄積バッファへの描画
	void DrawToAccumulationBuffer() override;
	//! @brief ライト定数バッファの設定
	void SetLightConstantBuffer() override;
};

//---------------------------------------------------------------------
//! @class PointLight
//! @brief 点光源クラス
//---------------------------------------------------------------------
class PointLight :public LightBase
{
public:
	float intensity; //!< 減衰率
	float range; //!< 影響範囲
	PointLight(const Vector3& pos, const Color& col, float inten, float rng)
		: LightBase(pos, col, LightType::Point) {
		intensity = inten;
		range = rng;
	}

protected:
	//! @brief 蓄積バッファへの描画
	void DrawToAccumulationBuffer() override;
	//! @brief ライト定数バッファの設定
	void SetLightConstantBuffer() override;
};
//---------------------------------------------------------------------
//! @class LightManager
//! @brief ゲーム内のライトを管理するマネージャークラス
//---------------------------------------------------------------------
class LightManager :public Object
{
private:
	std::vector<SafeSharedPtr<LightBase>> lights; //!< 管理するライトのリスト
	ShaderPs* light_blend_shader = nullptr; //!< ライトブレンド用シェーダー
	SafeSharedPtr<Texture> specular_accumulation_texture = nullptr; //!< スペキュラ蓄積テクスチャ
	SafeSharedPtr<Texture> diffuse_accumulation_texture = nullptr; //!< ディフューズ蓄積テクスチャ
	SafeSharedPtr<Texture> bloom_work_texture = nullptr; //!< ブルーム作業用テクスチャ
	static constexpr u8 REDUCTION_COUNT_MAX = 5; //!< ブルーム縮小回数の最大値
	std::array<std::pair<SafeSharedPtr<Texture>, SafeSharedPtr<Texture>>, REDUCTION_COUNT_MAX> bloom_reduction_textures; //!< ブルーム縮小テクスチャ配列
	mat4x4 camera_view_proj = mat4x4(physx::PxIdentity); //!< カメラのビュープロジェクション行列
	int lights_cbuffer_handle = -1; //!< ライト用定数バッファハンドル
	static constexpr u8 MAX_FORWARD_LIGHTS = 6; //!< フォワードライトの最大数
	SafeSharedPtr<Texture> sdr = nullptr; //!< SDRテクスチャ

public:
	LightManager() :Object(1) {}
	USING_SUPER(LightManager);

	//! @brief 初期化処理
	int Init() override;
	//! @brief ライトをマネージャーに追加する（スマートポインタ版）
	void AddLight(SafeSharedPtr<LightBase> light) {
		if (!light)
			return;
		if (!light->is_initialized)
			light->Init();
		light->my_manager = SafeStaticCast<LightManager>(SafeSharedPtr(shared_from_this()));
		lights.push_back(light);
	}
	//! @brief ライトを種別・パラメータで追加する
	void AddLight(LightType type, const Vector3& position, const Color& color, float range = 0, float intensity = 1.0f, Vector3 direction = { 0,-1,0 }) {
		auto light = SafeSharedPtr<LightBase>(nullptr);
		switch (type) {
		case LightType::Directional: {
			auto dir_light = make_safe_shared<DirectionalLight>(position, color, direction);
			light = dir_light;
			break;
		}
		case LightType::Point: {
			auto point_light = make_safe_shared<PointLight>(position, color, intensity, range);
			light = point_light;
			break;
		}
		}
		if (!light)
			return;
		light->type = type;
		light->position = position;
		light->color = color;
		if (!light->is_initialized)
			light->Init();
		light->my_manager = SafeStaticCast<LightManager>(SafeSharedPtr(shared_from_this()));
		lights.push_back(light);
	}
	//! @brief ライトをマネージャーから削除する
	void RemoveLight(SafeSharedPtr<LightBase> light) {
		if (!light)
			return;
		auto ite = std::find(lights.begin(), lights.end(), light);
		if (ite != lights.end()) {
			lights.erase(ite);
		}
	}
	//! @brief 管理中のライト一覧を取得する
	const std::vector<SafeSharedPtr<LightBase>>& GetLights() const {
		return lights;
	}
	//! @brief 描画処理
	void Draw() override;
	//! @brief 遅延描画処理
	void LateDraw() override;
	//! @brief デバッグ描画（遅延）
	void LateDebugDraw() override;
	//! @brief 終了処理
	void Exit() override;
	//! @brief カメラのビュープロジェクション行列を取得する
	const mat4x4& GetCameraViewProj() const { return camera_view_proj; }
};
