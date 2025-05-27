#pragma once
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
	static std::vector<physx::PxScene*> scenes;
public:


	static void Init();

	static void Exit();

	static physx::PxScene* AddScene();
	static void ReleaseScene(physx::PxScene* scene_);
	inline static physx::PxPhysics* GetPhysicsInstance() { return m_pPhysics; }
};

USING_PTR(Collider);
class HitInfo {
public:
	ColliderP collision = nullptr;
	ColliderP hit_collision = nullptr;
};

class HitCallBack : public physx::PxSimulationEventCallback {

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

	// 使わない関数は空実装
	void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}
	void onWake(physx::PxActor**, physx::PxU32) override {}
	void onSleep(physx::PxActor**, physx::PxU32) override {}
	void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, physx::PxU32) override {}
};


struct Material {
	static physx::PxMaterial* Metal_Default;
	static physx::PxMaterial* Rubber_Default;
	static physx::PxMaterial* Wood_Default;
	static physx::PxMaterial* Plastic_Default;
	static physx::PxMaterial* Glass_Default;
	static physx::PxMaterial* Concrete_Default;
	static physx::PxMaterial* Asphalt_Default;
	static physx::PxMaterial* Wool_Default;
	static physx::PxMaterial* Paper_Default;
};
