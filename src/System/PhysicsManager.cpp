#include "precompile.h"
#include "PhysicsManager.h"
#include "System/ObjBase.h"
#include <System/Components/Collider.h>

using namespace physx;

PxDefaultAllocator PhysicsManager::m_defaultAllocator;

PxDefaultErrorCallback PhysicsManager::m_defaultErrorCallback;

PxFoundation* PhysicsManager::m_pFoundation = nullptr;

PxPhysics* PhysicsManager::m_pPhysics = nullptr;

PxDefaultCpuDispatcher* PhysicsManager::m_pDispatcher = nullptr;

PxPvd* PhysicsManager::m_pPvd = nullptr;

std::vector<physx::PxScene*> PhysicsManager::scenes(0);


physx::PxMaterial* Material::Metal_Default = nullptr;
physx::PxMaterial* Material::Rubber_Default = nullptr;
physx::PxMaterial* Material::Wood_Default = nullptr;
physx::PxMaterial* Material::Plastic_Default = nullptr;
physx::PxMaterial* Material::Glass_Default = nullptr;
physx::PxMaterial* Material::Concrete_Default = nullptr;
physx::PxMaterial* Material::Asphalt_Default = nullptr;
physx::PxMaterial* Material::Wool_Default = nullptr;
physx::PxMaterial* Material::Paper_Default = nullptr;

HitCallBack hit_callback;

PxFilterFlags filtershader(PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize) {
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	}
	else {
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	}
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;


	return PxFilterFlag::eDEFAULT;
}
void PhysicsManager::Init()
{
	if (!(m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback))) {
		throw Exception("PhysXの初期化に失敗しました。", DEFAULT_EXCEPTION_PARAM);
	}
	// PVDと接続する設定
	if (m_pPvd = physx::PxCreatePvd(*m_pFoundation)) {
		// PVD側のデフォルトポートは5425
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

		int try_num = 0;
		//3回ほど試す
		while (try_num < 3) {
			//PVDとの接続を試みる
			bool success = m_pPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			if (success)
				break;
			//駄目なら少し待ってから再度試みる
			Sleep(10);
			try_num++;
		}
	}
	// Physicsのインスタンス化
	if (!(m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPvd))) {
		throw(Exception("PhysXのインスタンス生成に失敗しました。", DEFAULT_EXCEPTION_PARAM));
	}
	// 拡張機能用
	if (!PxInitExtensions(*m_pPhysics, m_pPvd)) {
		return (void)false;
	}
	// 処理に使うスレッドを指定する
	m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(16);
	physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
	scene_desc.simulationEventCallback = &hit_callback;

	Material::Metal_Default = m_pPhysics->createMaterial(0.6f, 0.5f, 0.2f);
	Material::Rubber_Default = m_pPhysics->createMaterial(1.1f, 0.8f, 0.7f);
	Material::Wood_Default = m_pPhysics->createMaterial(0.6f, 0.5f, 0.2f);
	Material::Plastic_Default = m_pPhysics->createMaterial(0.4f, 0.2f, 0.3f);
	Material::Glass_Default = m_pPhysics->createMaterial(0.6f, 0.5f, 0.2f);
	Material::Concrete_Default = m_pPhysics->createMaterial(1.0f, 0.8f, 0.2f);
	Material::Asphalt_Default = m_pPhysics->createMaterial(0.8f, 0.6f, 0.3f);
	Material::Wool_Default = m_pPhysics->createMaterial(0.7f, 0.5f, 0.05f);
	Material::Paper_Default = m_pPhysics->createMaterial(0.5f, 0.4f, 0.1f);

}

void PhysicsManager::Exit()
{
	//拡張機能を使わなくする
	PxCloseExtensions();
	for (auto& ite : scenes) {
		ite->release();
	}

	Material::Metal_Default->release();
	Material::Rubber_Default->release();
	Material::Wood_Default->release();
	Material::Plastic_Default->release();
	Material::Glass_Default->release();
	Material::Concrete_Default->release();
	Material::Asphalt_Default->release();
	Material::Wool_Default->release();
	Material::Paper_Default->release();
	m_pDispatcher->release();
	m_pPhysics->release();
	if (m_pPvd) {
		m_pPvd->disconnect();
		physx::PxPvdTransport* transport = m_pPvd->getTransport();
		m_pPvd->release();
		transport->release();
	}
	m_pFoundation->release();
}

physx::PxScene* PhysicsManager::AddScene()
{
	// シミュレーションする空間の単位でActorの追加などをここで行う
	// 空間の設定
	physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0, -9.81f, 0);

	scene_desc.filterShader = filtershader;
	scene_desc.cpuDispatcher = m_pDispatcher;
	scene_desc.simulationEventCallback = &hit_callback;
	scene_desc.flags |= PxSceneFlag::eENABLE_CCD;
	scene_desc.flags |= PxSceneFlag::eENABLE_PCM;
	scene_desc.flags |= PxSceneFlag::eENABLE_STABILIZATION;



	physx::PxScene* scene = nullptr;

	// 空間のインスタンス化
	scene = (m_pPhysics->createScene(scene_desc));
	// PVDの表示設定

	physx::PxPvdSceneClient* pvd_client;
	if (pvd_client = scene->getScenePvdClient()) {
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	scenes.push_back(scene);

	return scene;
}

void PhysicsManager::ReleaseScene(physx::PxScene* scene_)
{
	for (auto ite = scenes.begin(); ite != scenes.end();) {
		if (*ite == scene_) {
			(*ite)->release();
			ite = scenes.erase(ite);
			break;
		}
		ite++;
	}
}

void HitCallBack::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; i++)
	{
		PxContactPair contact = pairs[i];
		if (contact.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) // 衝突が発生したとき
		{
			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[0]->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[1]->userData));
			//std::weak_ptrへのポインタにすることで、userDataがnullptrになっていたり、shared_ptrが解放されていてもアクセスすることがない
			if (wp_a && wp_b) {
				//ヒット時に、オブジェクトを消しちゃうおバカちゃんたちがいるかもしれないので
				//あらかじめlockしてコールバック内で解放されないようにしておく
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.collision = sp_a->GetComponent<Collider>();
					hit_info0.hit_collision = sp_b->GetComponent<Collider>();
					sp_a->OnCollisionEnter(hit_info0); // オブジェクトAのヒット発生関数を呼ぶ
					HitInfo hit_info1;
					hit_info1.collision = sp_b->GetComponent<Collider>();
					hit_info1.hit_collision = sp_a->GetComponent<Collider>();
					sp_b->OnCollisionEnter(hit_info1); // オブジェクトBのヒット発生関数を呼ぶ
				}

			}
		}

		if (contact.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) // 衝突が続けて発生したとき
		{

			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[0]->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[1]->userData));
			if (wp_a && wp_b) {
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.collision = sp_a->GetComponent<Collider>();
					hit_info0.hit_collision = sp_b->GetComponent<Collider>();
					sp_a->OnCollisionStay(hit_info0); // オブジェクトAのヒット継続関数を呼ぶ
					HitInfo hit_info1;
					hit_info1.collision = sp_b->GetComponent<Collider>();
					hit_info1.hit_collision = sp_a->GetComponent<Collider>();
					sp_b->OnCollisionStay(hit_info1); // オブジェクトBのヒット継続関数を呼ぶ
				}
			}
		}

		if (contact.events & PxPairFlag::eNOTIFY_TOUCH_LOST) // 衝突が終了したとき
		{
			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[0]->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[1]->userData));
			if (wp_a && wp_b) {
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.collision = sp_a->GetComponent<Collider>();
					hit_info0.hit_collision = sp_b->GetComponent<Collider>();
					sp_a->OnCollisionExit(hit_info0); // オブジェクトAのヒット終了関数を呼ぶ
					HitInfo hit_info1;
					hit_info1.collision = sp_b->GetComponent<Collider>();
					hit_info1.hit_collision = sp_a->GetComponent<Collider>();
					sp_b->OnCollisionExit(hit_info1); // オブジェクトBのヒット終了関数を呼ぶ
				}
			}
		}
	}

}

void HitCallBack::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		PxTriggerPair contact = pairs[i];
		if (contact.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) // 衝突が発生したとき
		{
			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((contact.triggerActor->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((contact.otherActor->userData));
			//std::weak_ptrへのポインタにすることで、userDataがnullptrになっていたり、shared_ptrが解放されていてもアクセスすることがない
			if (wp_a && wp_b) {

				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.collision = sp_a->GetComponent<Collider>();
					hit_info0.hit_collision = sp_b->GetComponent<Collider>();
					sp_a->OnTriggerEnter(hit_info0); // オブジェクトAのトリガー発生関数を呼ぶ
					HitInfo hit_info1;
					hit_info1.collision = sp_b->GetComponent<Collider>();
					hit_info1.hit_collision = sp_a->GetComponent<Collider>();
					sp_b->OnTriggerEnter(hit_info1); // オブジェクトBのトリガー発生関数を呼ぶ
				}

			}
		}

		if (contact.status & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) // 衝突が続けて発生したとき
		{

			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((contact.triggerActor->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((contact.otherActor->userData));
			if (wp_a && wp_b) {
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.collision = sp_a->GetComponent<Collider>();
					hit_info0.hit_collision = sp_b->GetComponent<Collider>();
					sp_a->OnTriggerStay(hit_info0); // オブジェクトAのトリガー継続関数を呼ぶ
					HitInfo hit_info1;
					hit_info1.collision = sp_b->GetComponent<Collider>();
					hit_info1.hit_collision = sp_a->GetComponent<Collider>();
					sp_b->OnTriggerStay(hit_info1); // オブジェクトBのトリガー継続関数を呼ぶ
				}
			}
		}

		if (contact.status & PxPairFlag::eNOTIFY_TOUCH_LOST) // 衝突が終了したとき
		{
			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((contact.triggerActor->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((contact.otherActor->userData));
			if (wp_a && wp_b) {
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.collision = sp_a->GetComponent<Collider>();
					hit_info0.hit_collision = sp_b->GetComponent<Collider>();
					sp_a->OnTriggerExit(hit_info0); // オブジェクトAのトリガー終了関数を呼ぶ
					HitInfo hit_info1;
					hit_info1.collision = sp_b->GetComponent<Collider>();
					hit_info1.hit_collision = sp_a->GetComponent<Collider>();
					sp_b->OnTriggerExit(hit_info1); // オブジェクトBのトリガー終了関数を呼ぶ
				}
			}
		}
	}
}
