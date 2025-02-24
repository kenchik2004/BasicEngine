#include "precompile.h"
#include "PhysicsManager.h"
#include "System/ObjBase.h"

using namespace physx;

PxDefaultAllocator PhysicsManager::m_defaultAllocator;

PxDefaultErrorCallback PhysicsManager::m_defaultErrorCallback;

PxFoundation* PhysicsManager::m_pFoundation = nullptr;

PxPhysics* PhysicsManager::m_pPhysics = nullptr;

PxDefaultCpuDispatcher* PhysicsManager::m_pDispatcher = nullptr;

PxPvd* PhysicsManager::m_pPvd = nullptr;

std::vector<physx::PxScene*> PhysicsManager::scenes(0);

HitCallBack hit_callback;

PxFilterFlags filtershader(PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize) {

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	// �Փˎ��̃C�x���g��L����
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;

	return PxFilterFlag::eDEFAULT;
}
void PhysicsManager::Init()
{
	if (!(m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback))) {
		throw Exception("PhysX�̏������Ɏ��s���܂����B", DEFAULT_EXCEPTION_PARAM);
	}
	// PVD�Ɛڑ�����ݒ�
	if (m_pPvd = physx::PxCreatePvd(*m_pFoundation)) {
		// PVD���̃f�t�H���g�|�[�g��5425
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

		int try_num = 0;
		//3��قǎ���
		while (try_num < 3) {
			//PVD�Ƃ̐ڑ������݂�
			bool success = m_pPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			if (success)
				break;
			//�ʖڂȂ班���҂��Ă���ēx���݂�
			Sleep(10);
			try_num++;
		}
	}
	// Physics�̃C���X�^���X��
	if (!(m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPvd))) {
		throw(Exception("PhysX�̃C���X�^���X�����Ɏ��s���܂����B", DEFAULT_EXCEPTION_PARAM));
	}
	// �g���@�\�p
	if (!PxInitExtensions(*m_pPhysics, m_pPvd)) {
		return (void)false;
	}
	// �����Ɏg���X���b�h���w�肷��
	m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(16);
	physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
	scene_desc.simulationEventCallback = &hit_callback;

}

void PhysicsManager::Exit()
{
	//�g���@�\���g��Ȃ�����
	PxCloseExtensions();
	for (auto& ite : scenes) {
		if (!ite)
			ite->release();
	}

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
	// �V�~�����[�V���������Ԃ̒P�ʂ�Actor�̒ǉ��Ȃǂ������ōs��
	// ��Ԃ̐ݒ�
	physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0, -9.81f, 0);

	scene_desc.filterShader = filtershader;
	scene_desc.cpuDispatcher = m_pDispatcher;
	scene_desc.simulationEventCallback = &hit_callback;
	scene_desc.flags |= PxSceneFlag::eENABLE_CCD;
	scene_desc.flags |= PxSceneFlag::eENABLE_PCM;
	scene_desc.flags |= PxSceneFlag::eENABLE_STABILIZATION;


	physx::PxScene* scene = nullptr;

	// ��Ԃ̃C���X�^���X��
	scene = (m_pPhysics->createScene(scene_desc));
	// PVD�̕\���ݒ�

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
		}
	}
}

void HitCallBack::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	SceneManager::GetCurrentScene()->GetPhysicsScene()->lockRead(); // PhysX �̃X���b�h�����b�N
	for (physx::PxU32 i = 0; i < nbPairs; i++)
	{
		PxContactPair contact = pairs[i];
		if (contact.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) // �Փ˂����������Ƃ�
		{
			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[0]->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[1]->userData));
			//std::weak_ptr�ւ̃|�C���^�ɂ��邱�ƂŁAuserData��nullptr�ɂȂ��Ă�����Ashared_ptr���������Ă��Ă��A�N�Z�X���邱�Ƃ��Ȃ�
			if (wp_a && wp_b) {
				//�q�b�g���ɁA�I�u�W�F�N�g���������Ⴄ���o�J����񂽂������邩������Ȃ��̂�
				//���炩����lock���ăR�[���o�b�N���ŉ������Ȃ��悤�ɂ��Ă���
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.hit_collision = sp_b;
					sp_a->OnCollisionEnter(hit_info0); // �I�u�W�F�N�gA�̃q�b�g�����֐����Ă�
					HitInfo hit_info1;
					hit_info1.hit_collision = sp_a;
					sp_b->OnCollisionEnter(hit_info1); // �I�u�W�F�N�gB�̃q�b�g�����֐����Ă�
				}

			}
		}

		if (contact.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) // �Փ˂������Ĕ��������Ƃ�
		{

			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[0]->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[1]->userData));
			if (wp_a && wp_b) {
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.hit_collision = sp_b;
					sp_a->OnCollisionStay(hit_info0); // �I�u�W�F�N�gA�̃q�b�g�p���֐����Ă�
					HitInfo hit_info1;
					hit_info1.hit_collision = sp_a;
					sp_b->OnCollisionStay(hit_info1); // �I�u�W�F�N�gB�̃q�b�g�p���֐����Ă�
				}
			}
		}

		if (contact.events & PxPairFlag::eNOTIFY_TOUCH_LOST) // �Փ˂��I�������Ƃ�
		{
			auto wp_a = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[0]->userData));
			auto wp_b = static_cast<std::weak_ptr<ObjBase>*>((pairHeader.actors[1]->userData));
			if (wp_a && wp_b) {
				auto sp_a = wp_a->lock();
				auto sp_b = wp_b->lock();
				if (sp_a && sp_b) {
					HitInfo hit_info0;
					hit_info0.hit_collision = sp_b;
					sp_a->OnCollisionExit(hit_info0); // �I�u�W�F�N�gA�̃q�b�g�I���֐����Ă�
					HitInfo hit_info1;
					hit_info1.hit_collision = sp_a;
					sp_b->OnCollisionExit(hit_info1); // �I�u�W�F�N�gB�̃q�b�g�I���֐����Ă�
				}
			}
		}
	}
	SceneManager::GetCurrentScene()->GetPhysicsScene()->unlockRead(); // PhysX �̃X���b�h���A�����b�N

}

void HitCallBack::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
}
