//---------------------------------------------------------------------------
//! @file   MeshCollider.h
//! @brief  メッシュ形状の衝突判定を提供するMeshColliderコンポーネント
//---------------------------------------------------------------------------
#pragma once
#include "System/Components/Collider.h"


USING_PTR(ModelRenderer);
USING_PTR(MeshCollider);
//---------------------------------------------------------------------
//! @class MeshCollider
//! @brief モデルのポリゴンメッシュを使ったコライダーコンポーネント
//---------------------------------------------------------------------
class MeshCollider :
	public Collider
{
public:
	USING_SUPER(MeshCollider);
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
	ModelRendererWP model;                          //!< 対象のModelRendererへの弱参照
	physx::PxTriangleMeshGeometry mesh{};           //!< PhysXの三角形メッシュジオメトリ
	bool attached = false;                          //!< モデルへのアタッチ済みフラグ
	MV1_REF_POLYGONLIST* ref_poly_ = nullptr;       //!< ポリゴンデータ
};

