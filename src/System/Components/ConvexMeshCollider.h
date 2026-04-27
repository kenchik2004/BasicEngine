
//---------------------------------------------------------------------------
//! @file   ConvexMeshCollider.h
//! @brief  凸メッシュ形状の衝突判定を提供するConvexMeshColliderコンポーネント
//---------------------------------------------------------------------------
#pragma once
#include "System/Components/Collider.h"


USING_PTR(ModelRenderer);
USING_PTR(ConvexMeshCollider);
//---------------------------------------------------------------------
//! @class ConvexMeshCollider
//! @brief モデルの凸メッシュを使ったコライダーコンポーネント
//---------------------------------------------------------------------
class ConvexMeshCollider :
	public Collider
{

public:
	USING_SUPER(ConvexMeshCollider);
	//! @brief 初期化処理
	int Init() override;
	//! @brief 物理演算前処理（メッシュの更新）
	void PrePhysics() override;
	//! @brief デバッグ描画処理
	void DebugDraw() override;
	//! @brief 更新処理
	void Update() override;
	//! @brief モデルにアタッチする
	void AttachToModel();
	//! @brief 終了処理
	void Exit() override;

private:
	ModelRendererWP model;                         //!< 対象のModelRendererへの弱参照
	physx::PxConvexMeshGeometry mesh{};            //!< PhysXの凸メッシュジオメトリ
	bool attached = false;                         //!< モデルへのアタッチ済みフラグ
	MV1_REF_POLYGONLIST* ref_poly_ = nullptr;       //!< ポリゴンデータ
};

