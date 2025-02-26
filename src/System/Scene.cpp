#include "precompile.h"
#include "System/Scene.h"
#include "System/ObjBase.h"




void Scene::Physics()
{
	if (!physics_scene)
		throw Exception("PhysX�V�[�����Ȃ����S!!���o�����S!", DEFAULT_EXCEPTION_PARAM);

	//�V�~�����[�V����
	in_simulation = true;
	physics_scene->simulate(Time::FixedDeltaTime());
	//�V�~�����[�V������҂�
	physics_scene->fetchResults(true);
	in_simulation = false;
	//�����҂��֐��̌Ăяo��
	for (auto& func : waiting_functions) {
		func();
	}
	waiting_functions.clear();

	//�V�~�����[�V�����I����A���S�ɃA�N�^�[���폜����
	for (auto actor : waiting_remove_actors) {

		physics_scene->removeActor(*actor);
		actor->release();
		//��x�ƎQ�Ƃ��邱�Ƃ��Ȃ��悤�A��������nullptr�����Ă���
		actor = nullptr;
	}
	for (auto shape : waiting_remove_shapes) {
		shape->release();
		//��x�ƎQ�Ƃ��邱�Ƃ��Ȃ��悤�A��������nullptr�����Ă���
		shape = nullptr;
	}
	//�S���������̂ł������肳����
	waiting_remove_actors.clear();
	waiting_remove_shapes.clear();
}

void Scene::DeleteActor(physx::PxRigidActor* actor)
{
	if (!actor)
		return;
	physics_scene->lockWrite();// PhysX �̃X���b�h�����b�N

	//�ȍ~�AHit�����Ȃǂɓ���Ȃ��悤userData��������āAnullptr�ɂ��Ă���
	auto wp = static_cast<std::weak_ptr<ObjBase>*>(actor->userData);
	actor->userData = nullptr;
	if (wp->lock())
		delete wp;
	////�A�N�^�[�������Ă���V�F�C�v����S���擾
	//physx::PxU32 num_shapes = actor->getNbShapes();
	//std::vector<physx::PxShape*> shapes(num_shapes);
	//actor->getShapes(shapes.data(), num_shapes);

	////�S�ẴV�F�C�v�ɑ΂��A�폜�\��̃t���O�𗧂ĂĂ���
	//for (auto shape : shapes) {
	//	physx::PxFilterData filter_data;
	//	filter_data = { 0,0,0,0 }; // �폜�Ώۃ}�[�N(�t�B���^�[�őS�r�b�g0�́A�����蔻�薳����)
	//	shape->setSimulationFilterData(filter_data);
	//}
	//�폜�\��A�N�^�[�ɒǉ��o�^
	waiting_remove_actors.push_back(actor);

	physics_scene->unlockWrite();// PhysX �̃X���b�h���A�����b�N
}

void Scene::DeleteShape(physx::PxShape* shape)
{
	if (!shape)
		return;
	auto wp = static_cast<std::weak_ptr<Collider>*>(shape->userData);
	shape->userData = nullptr;
	if (wp->lock())
		delete wp;

	auto body = shape->getActor();
	if (body)
		body->detachShape(*shape);
	waiting_remove_shapes.push_back(shape);
}


void Scene::Destroy()
{

	for (auto& obj : objects) {
		while (auto comp = obj->GetComponent<Component>()) {
			comp->RemoveThisComponent();
		}
		obj->Exit();
		obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
	}
	objects.clear();
	if (!physics_scene)
		throw (Exception("PhysX�V�[�����Ȃ����S!!���o�����S!", DEFAULT_EXCEPTION_PARAM));
	Physics();



}

void Scene::DestroyPhysics()
{
	//PhysX�V�[�����폜(relese)
	PhysicsManager::ReleaseScene(physics_scene);
	physics_scene = nullptr;
}
