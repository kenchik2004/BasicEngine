//---------------------------------------------------------------------------
//! @file   CapsuleCollider.h
//! @brief  カプセル形状の衝突判定を提供するCapsuleColliderコンポーネント
//---------------------------------------------------------------------------
#pragma once
#include "Collider.h"

USING_PTR(CapsuleCollider);
//---------------------------------------------------------------------
//! @class CapsuleCollider
//! @brief カプセル形状のコライダーコンポーネント
//---------------------------------------------------------------------
class CapsuleCollider :
	public Collider
{
public:
	USING_SUPER(CapsuleCollider);
	//! @brief コンストラクタ
	//! @param height_     カプセルの高さ
	//! @param radius_     カプセルの半径
	//! @param pos         コライダーのローカル座標
	//! @param rot         コライダーのローカル回転
	//! @param is_trigger_ トリガーとして扱うか
	//! @param layer_      衝突レイヤー
	//! @param hit_group_  ヒットグループ
	CapsuleCollider(float height_ = 1.0f, float radius_ = 0.5f,
		Vector3 pos = { 0,0,0 },Quaternion rot = { 0,0,0,1 },
		bool is_trigger_ = false,
		Collider::Layer layer_ = Collider::Layer::Default,
		u32 hit_group_ = Collider::Layer::All)
		:Collider(pos, rot, is_trigger_, layer_, hit_group_),
		height(height_), radius(radius_) {
	}
	//! @brief 初期化処理
	int Init()override;
	//! @brief 物理演算前処理（形状の更新）
	void PrePhysics()override;
	//! @brief デバッグ描画処理
	void DebugDraw() override;
	float height = 1.0f; //!< カプセルの高さ
	float radius = 0.5f; //!< カプセルの半径
};

