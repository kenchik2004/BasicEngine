//----------------------------------------------------------------------------
//!	@file	Model.h
//!	@brief	モデル、モデルソース、アニメーション関連のクラス定義。3Dモデルの読み込み、描画、アニメーションを管理。
//!　@author　岩野 健太郎
//----------------------------------------------------------------------------

#pragma once

//----------------------------------------------------
// @brief モデルソースを表すクラス。モデルファイルの読み込みと物理メッシュの生成を管理。
//----------------------------------------------------
class ModelSource
{
	friend class ModelManager; // ModelManager クラスが private メンバーにアクセスできるようにする。
	std::string path; // モデルファイルのパス
	std::string name; // モデルソースの名前
	int handle = -1; // DXLib のモデルハンドル
	bool is_initialized = false; // 初期化済みかどうか
	physx::PxConvexMesh* convex_mesh = nullptr; // 凸メッシュ
	physx::PxTriangleMesh* triangle_mesh = nullptr; // 三角メッシュ
	MV1_REF_POLYGONLIST ref_poly_{}; // 参照ポリゴンリスト
	bool polygons_loaded = false; // ポリゴンが読み込まれたかどうか
	std::vector<Material*> materials = std::vector<Material*>(0); // マテリアル配列
public:
	//----------------------------------------------------
	// @brief 初期化する。
	//----------------------------------------------------
	void Init();
	//----------------------------------------------------
	// @brief ポリゴンを取得する。
	// @return 参照ポリゴンリストのポインタ。
	//----------------------------------------------------
	MV1_REF_POLYGONLIST* GetPolygon();
	//----------------------------------------------------
	// @brief 凸メッシュを取得または作成する。
	// @return 凸メッシュのポインタ。
	//----------------------------------------------------
	physx::PxConvexMesh* GetOrCreateConvexMesh();
	//----------------------------------------------------
	// @brief 三角メッシュを取得または作成する。
	// @return 三角メッシュのポインタ。
	//----------------------------------------------------
	physx::PxTriangleMesh* GetOrCreateTriangleMesh();
	//----------------------------------------------------
	// @brief デストラクタ。モデルとメッシュを解放。
	//----------------------------------------------------
	~ModelSource() { MV1DeleteModel(handle); if (convex_mesh)convex_mesh->release(); if (triangle_mesh)triangle_mesh->release(); }
};

//----------------------------------------------------
// @brief アニメーションソースを表すクラス。アニメーションファイルの読み込みを管理。
//----------------------------------------------------
class AnimSource
{
	friend class ModelManager; // ModelManager クラスが private メンバーにアクセスできるようにする。
	std::string path; // アニメーションファイルのパス
	std::string name; // アニメーションソースの名前
	int handle = -1; // DXLib のモデルハンドル
	int index = -1; // アニメーションインデックス
public:
	//----------------------------------------------------
	// @brief デストラクタ。モデルを解放。
	//----------------------------------------------------
	~AnimSource() { MV1DeleteModel(handle); }
};

//----------------------------------------------------
// @brief モデルを表すクラス。モデルの描画と物理メッシュの取得を管理。
//----------------------------------------------------
class Model
{
	friend class SceneManager; // SceneManager クラスが private メンバーにアクセスできるようにする。
	friend class ModelManager; // ModelManager クラスが private メンバーにアクセスできるようにする。
	friend class ModelRenderer; // ModelRenderer クラスが private メンバーにアクセスできるようにする。
	friend class Animator; // Animator クラスが private メンバーにアクセスできるようにする。
	std::string name; // モデル名
	int handle = -1; // DXLib のモデルハンドル
	ModelSource* original = nullptr; // 元のモデルソース
	std::vector<Material*> materials = std::vector<Material*>(0); // マテリアル配列

	static inline int instance = 0; // インスタンス数

public:
	bool use_lighting = true; // ライティングを使用するかどうか


	//----------------------------------------------------
	// @brief ハンドルを取得する。
	// @return DxLib のモデルハンドル。
	//----------------------------------------------------
	int GetHandle() const { return handle; }

	//----------------------------------------------------
	// @brief ポリゴンを取得する。
	// @return 参照ポリゴンリストのポインタ。
	//----------------------------------------------------
	MV1_REF_POLYGONLIST* GetPolygon() { return original->GetPolygon(); }
	//----------------------------------------------------
	// @brief 凸メッシュを取得する。
	// @return 凸メッシュのポインタ。
	//----------------------------------------------------
	physx::PxConvexMesh* GetConvexMesh();
	//----------------------------------------------------
	// @brief 三角メッシュを取得する。
	// @return 三角メッシュのポインタ。
	//----------------------------------------------------
	physx::PxTriangleMesh* GetTriangleMesh();
	//----------------------------------------------------
	// @brief コンストラクタ。
	//----------------------------------------------------
	Model();
	//----------------------------------------------------
	// @brief コピーコンストラクタ。
	// @param other コピー元の Model オブジェクト。
	//----------------------------------------------------
	Model(const Model& other) {
		name = other.name; // 名前をコピー
		handle = other.handle; // ハンドルをコピー
		instance++; // インスタンス数をインクリメント
	}
	//----------------------------------------------------
	// @brief デストラクタ。モデルを解放。
	//----------------------------------------------------
	~Model() {
		MV1DeleteModel(handle); // モデルを削除
		instance--; // インスタンス数をデクリメント
	}
	//----------------------------------------------------
	// @brief モデルを描画する。
	// @param to_gbuffer GBuffer に描画するかどうか。
	//----------------------------------------------------
	void Draw(bool to_gbuffer = false);
};

//----------------------------------------------------
// @brief アニメーションを表すクラス。アニメーションの再生とコールバックを管理。
//----------------------------------------------------
class Animation
{
	static inline int instance = 0; // インスタンス数
	friend class ModelManager; // ModelManager クラスが private メンバーにアクセスできるようにする。
	friend class Animator; // Animator クラスが private メンバーにアクセスできるようにする。
	std::string name; // アニメーション名
	int handle = -1; // DXLib のモデルハンドル
	int index = -1; // アニメーションインデックス
	int attached_index = -1; // アタッチされたインデックス
	float total_time = 0; // 総時間
	float current_time = 0; // 現在の時間
	float blend_rate = 0.0f; // ブレンドレート
	struct AnimationCallBack { // アニメーションコールバック構造体
		float ex_frame = 0; // 実行フレーム
		std::function<void()> function = nullptr; // コールバック関数
		bool is_executed = false; // 実行済みかどうか
	};
	std::vector<AnimationCallBack> call_backs; // コールバック配列
	std::unordered_map<std::string, size_t> method_names; // メソッド名マップ
public:
	//----------------------------------------------------
	// @brief コンストラクタ。
	//----------------------------------------------------
	Animation() { instance++; }
	//----------------------------------------------------
	// @brief コピーコンストラクタ。
	// @param other コピー元の Animation オブジェクト。
	//----------------------------------------------------
	Animation(const Animation& other) {
		name = other.name; // 名前をコピー
		handle = other.handle; // ハンドルをコピー
		index = other.index; // インデックスをコピー
		total_time = other.total_time; // 総時間をコピー
		instance++; // インスタンス数をインクリメント
	}
	//----------------------------------------------------
	// @brief アニメーションを更新する。
	// @param speed 再生速度。
	// @param check_call_backs コールバックをチェックするかどうか。
	//----------------------------------------------------
	void Update(float speed = 1.0f, bool check_call_backs = true);
	//----------------------------------------------------
	// @brief コールバックを初期化する。
	//----------------------------------------------------
	void InitCallBacks();
	//----------------------------------------------------
	// @brief コールバックを設定する。
	// @param call_back コールバック関数。
	// @param execute_frame 実行フレーム。
	// @param method_name メソッド名。
	//----------------------------------------------------
	void SetCallBack(std::function<void()>& call_back, float execute_frame, std::string_view method_name);
	//----------------------------------------------------
	// @brief コールバックをリセットする。
	// @param method_name メソッド名。
	//----------------------------------------------------
	void ResetCallBack(std::string_view method_name);
	//----------------------------------------------------
	// @brief デストラクタ。モデルを解放。
	//----------------------------------------------------
	~Animation() { MV1DeleteModel(handle); instance--; }
};

