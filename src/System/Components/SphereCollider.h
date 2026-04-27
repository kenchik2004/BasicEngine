//---------------------------------------------------------------------------
//! @file   SphereCollider.h
//! @brief  球形状の衝突判定を提供するSphereColliderコンポーネント
//---------------------------------------------------------------------------
#pragma once
#include "Collider.h"

USING_PTR(SphereCollider);
//---------------------------------------------------------------------
//! @class SphereCollider
//! @brief 球形状のコライダーコンポーネント
//---------------------------------------------------------------------
class SphereCollider :
    public Collider
{
public:
    USING_SUPER(SphereCollider);
    //! @brief コンストラクタ
    //! @param pos         コライダーのローカル座標
    //! @param rot         コライダーのローカル回転
    //! @param radius_     球の半径
    //! @param is_trigger_ トリガーとして扱うか
    //! @param layer_      衝突レイヤー
    //! @param hit_group_  ヒットグループ
    SphereCollider(Vector3 pos = { 0,0,0 }, Quaternion rot = { 0,0,0,1 }, float radius_ = 0.5f, bool is_trigger_ = false, Layer layer_ = Layer::Default, u8 hit_group_ = Layer::All)
		:Collider(pos, rot, is_trigger_, layer_, hit_group_), radius(radius_) {
	}
    //! @brief 初期化処理
    int Init() override;
    //! @brief 物理演算前処理（形状の更新）
    void PrePhysics() override;
    //! @brief デバッグ描画処理
    void DebugDraw() override;

    //! @brief 球の半径への参照を返す
    const float& Radius() { return radius; }
    float radius = 0.5f; //!< 球の半径
private:


};

