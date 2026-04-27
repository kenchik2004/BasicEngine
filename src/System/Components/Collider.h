//---------------------------------------------------------------------------
//! @file   Collider.h
//! @brief  衝突判定の基底クラスであるColliderコンポーネント
//---------------------------------------------------------------------------
#pragma once

USING_PTR(RigidBody);
USING_PTR(Collider);
//---------------------------------------------------------------------
//! @class Collider
//! @brief 各種コライダーの基底クラス。PhysXを用いた衝突判定を管理する
//---------------------------------------------------------------------
class Collider :
	public Component
{
public:
	//! @brief 衝突レイヤー定義
	enum Layer :physx::PxU32 {
		None    = 0,         //!< なし
		Default = 1,         //!< デフォルトレイヤー
		Terrain = 1 << 1,   //!< 地形レイヤー
		Wepon   = 1 << 2,   //!< 武器レイヤー
		Enemy   = 1 << 3,   //!< 敵レイヤー
		Player  = 1 << 4,   //!< プレイヤーレイヤー
		Vehicle = 1 << 5,   //!< 乗り物レイヤー

		All = UINT32_MAX,    //!< 全レイヤー
	};
	USING_SUPER(Collider);
	//! @brief コンストラクタ
	//! @param pos         コライダーのローカル座標
	//! @param rot         コライダーのローカル回転
	//! @param is_trigger_ トリガーとして扱うか
	//! @param layer_      所属する衝突レイヤー
	//! @param hit_group_  ヒット可能なグループ
	Collider(Vector3 pos = { 0,0,0 }, Quaternion rot = { 0,0,0,1 }, bool is_trigger_ = false, Layer layer_ = Layer::Default, u32 hit_group_ = Layer::All)
		:position(pos), rotation(rot), is_trigger(is_trigger_), collision_group(layer_), hit_group(hit_group_) {
	}
	bool is_trigger = false; //!< トリガーとして動作させるか
	//! @brief コンストラクト処理
	void Construct() override;
	//! @brief スリープ時の処理
	void OnSleep() override;
	//! @brief 起床時の処理
	void OnWakeUp() override;
	//! @brief 初期化処理
	int Init() override;
	//! @brief 終了処理
	void Exit() override;
	Vector3 position = { 0,0,0 };          //!< コライダーのローカル座標オフセット
	Quaternion rotation = { 0,0,0,1 };     //!< コライダーのローカル回転オフセット
	//! @brief 接続しているRigidBodyを取得する
	RigidBodyP GetRigidBody() { return rigidbody.lock(); }
	//! @brief 指定フレームにアタッチする
	void AttachToModel(int attach_index);
	//! @brief 物理マテリアルを設定する
	void SetMaterial(physx::PxMaterial* new_mat);

	//! @brief 衝突レイヤーを設定する
	void SetLayer(Layer layer);
	//! @brief 衝突レイヤーを取得する
	Layer GetLayer() { return collision_group; }
	//! @brief ヒットグループを取得する
	u32 GetHitGroup() { return hit_group; }
	//! @brief ヒットグループを設定する
	void SetHitGroup(u32 hit_layer);
protected:
	u32 hit_group = All;                   //!< ヒット可能なグループマスク
	Layer collision_group = Default;       //!< 所属する衝突レイヤー
	bool attach_to_model = false;          //!< モデルのフレームにアタッチするか
	int model_attach_index = -1;           //!< アタッチ先のフレームインデックス
	RigidBodyWP rigidbody;                 //!< 接続しているRigidBodyへの弱参照
	physx::PxShape* shape;                 //!< PhysXシェイプへのポインタ

	//! @brief コライダーのPhysXトランスフォームを生成する
	physx::PxTransform MakeCollisionTransform();

};

