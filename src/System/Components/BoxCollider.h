//---------------------------------------------------------------------------
//! @file   BoxCollider.h
//! @brief  ボックス形状の衝突判定を提供するBoxColliderコンポーネント
//---------------------------------------------------------------------------
#pragma once
#include "Collider.h"
USING_PTR(BoxCollider);
//---------------------------------------------------------------------
//! @class BoxCollider
//! @brief 直方体形状のコライダーコンポーネント
//---------------------------------------------------------------------
class BoxCollider :
	public Collider
{
public:
	USING_SUPER(BoxCollider);
	//! @brief コンストラクタ
	//! @param pos       コライダーのローカル座標
	//! @param rot       コライダーのローカル回転
	//! @param ext       ボックスの各軸方向の半サイズ
	//! @param is_trigger_ トリガーとして扱うか
	//! @param layer_    衝突レイヤー
	//! @param hit_group_ ヒットグループ
	BoxCollider(Vector3 pos = { 0,0,0 }, Quaternion rot = { 0,0,0,1 }, Vector3 ext = { 1.0f,1.0f,1.0f }, bool is_trigger_ = false,
		Layer layer_ = Layer::Default, u8 hit_group_ = Layer::All)
		:Collider(pos, rot, is_trigger_, layer_, hit_group_), extension(ext) {
	}
	//! @brief 初期化処理
	int Init() override;
	//! @brief 物理演算前処理（形状の更新）
	void PrePhysics() override;
	//! @brief デバッグ描画処理
	void DebugDraw() override;
	Vector3 extension = { 1.0f,1.0f,1.0f }; //!< ボックスの各軸方向の半サイズ
};

