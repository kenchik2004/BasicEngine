//---------------------------------------------------------------------------
//! @file   PhysicsManager.h
//! @brief  PhysXを用いた物理エンジンの初期化・管理を行うマネージャー
//---------------------------------------------------------------------------
#pragma once

using RayCastInfo = physx::PxRaycastBuffer;

//---------------------------------------------------------------------
//! @class Ray
//! @brief レイキャスト用のレイ情報を保持するクラス
//---------------------------------------------------------------------
class Ray {
public:
	Vector3 position;	//!< レイの発射位置
	Vector3 direction;	//!< レイの方向（正規化済みベクトル）
	float length;		//!< レイの長さ
};

//---------------------------------------------------------------------
//! @class PhysicsManager
//! @brief PhysXを使用した物理シミュレーションの初期化・管理を行うクラス
//---------------------------------------------------------------------
class PhysicsManager
{
	static physx::PxDefaultAllocator m_defaultAllocator;
	// エラー時用のコールバックでエラー内容が入ってる
	static physx::PxDefaultErrorCallback m_defaultErrorCallback;
	// 上位レベルのSDK(PxPhysicsなど)をインスタンス化する際に必要
	static physx::PxFoundation* m_pFoundation;
	// 実際に物理演算を行う
	static physx::PxPhysics* m_pPhysics;
	// シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる
	static physx::PxDefaultCpuDispatcher* m_pDispatcher;
	// PVDと通信する際に必要
	static physx::PxPvd* m_pPvd;
	static std::vector<physx::PxScene*> scenes;	//!< 管理中の物理シーンリスト
public:

	//! @brief 初期化処理
	static void Init();

	//! @brief 終了処理
	static void Exit();

	//! @brief 物理シーンを追加する
	static physx::PxScene* AddScene();
	//! @brief 物理シーンを解放する
	static void ReleaseScene(physx::PxScene* scene_);
	//! @brief PhysXのインスタンスを取得する
	inline static physx::PxPhysics* GetPhysicsInstance() { return m_pPhysics; }
};

USING_PTR(Collider);

//---------------------------------------------------------------------
//! @class HitInfo
//! @brief 衝突情報を保持するクラス
//---------------------------------------------------------------------
class HitInfo {
public:
	Vector3 hit_position;		//!< 衝突位置
	ColliderP collision = nullptr;	//!< 自身のコライダー
	ColliderP hit_collision = nullptr;	//!< 衝突相手のコライダー
};

//---------------------------------------------------------------------
//! @class HitCallBack
//! @brief PhysXの衝突イベントコールバッククラス
//---------------------------------------------------------------------
class HitCallBack : public physx::PxSimulationEventCallback {

	//! @brief 接触イベントが発生したときに呼ばれる
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
	//! @brief トリガーイベントが発生したときに呼ばれる
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

	// 使わない関数は空実装
	void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}
	void onWake(physx::PxActor**, physx::PxU32) override {}
	void onSleep(physx::PxActor**, physx::PxU32) override {}
	void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, physx::PxU32) override {}
};

//---------------------------------------------------------------------
//! @class RaycastCallback
//! @brief レイキャスト用フィルタリングコールバッククラス
//---------------------------------------------------------------------
class RaycastCallback : public physx::PxQueryFilterCallback
{
public:
	// 衝突したとき呼ばれる（本体）
	physx::PxQueryHitType::Enum preFilter(
		const  physx::PxFilterData& filterData,
		const  physx::PxShape* shape,
		const  physx::PxRigidActor* actor,
		physx::PxHitFlags& queryFlags
	) override
	{
		// 例: それぞれのshapeに設定されたフィルターデータを取得
		const  physx::PxFilterData& shapeFilter = shape->getQueryFilterData();

		// たとえば word0 を "自分のレイヤー"、word1 を "衝突対象のマスク" として使う
		// もしマスクが合っていなければ無視
		if ((filterData.word0 & shapeFilter.word1) == 0)
			return  physx::PxQueryHitType::eNONE;

		return  physx::PxQueryHitType::eBLOCK; // 通常ヒット
	}

	physx::PxQueryHitType::Enum postFilter(
		const physx::PxFilterData& filterData,
		const physx::PxQueryHit& hit,
		const physx::PxShape* shape,
		const physx::PxRigidActor* actor) override
	{
		return  physx::PxQueryHitType::eBLOCK;
	}
	static RaycastCallback* instance() {
		static RaycastCallback instance_;
		return &instance_;

	}
};


//---------------------------------------------------------------------
//! @struct PhysicMaterial
//! @brief  物理マテリアルのプリセット定数を保持する構造体
//---------------------------------------------------------------------
struct PhysicMaterial {
	static physx::PxMaterial* Default;		//!< デフォルトマテリアル
	static physx::PxMaterial* HighFriction;	//!< 摩擦係数が大きいマテリアル
	static physx::PxMaterial* ZeroFriction;	//!< 摩擦係数ゼロのマテリアル
	static physx::PxMaterial* Metal;		//!< 金属マテリアル
	static physx::PxMaterial* Rubber;		//!< ゴムマテリアル
	static physx::PxMaterial* Wood;			//!< 木材マテリアル
	static physx::PxMaterial* Plastic;		//!< プラスチックマテリアル
	static physx::PxMaterial* Glass;		//!< ガラスマテリアル
	static physx::PxMaterial* Concrete;		//!< コンクリートマテリアル
	static physx::PxMaterial* Asphalt;		//!< アスファルトマテリアル
	static physx::PxMaterial* Wool;			//!< 羊毛マテリアル
	static physx::PxMaterial* Paper;		//!< 紙マテリアル
};
