#include "precompile.h"
#include "System/Scene.h"
#include "System/ObjBase.h"




void Scene::Physics()
{
	if (!physics_scene)
		throw Exception("PhysXシーンがないンゴ!!ヤバいンゴ!", DEFAULT_EXCEPTION_PARAM);

	//シミュレーション
	in_simulation = true;
	physics_scene->simulate(Time::FixedDeltaTime() * physics_timescale);
	//シミュレーションを待つ
	physics_scene->fetchResults(true);
	in_simulation = false;
	//処理待ち関数の呼び出し
	for (auto& func : waiting_functions) {
		func();
	}
	waiting_functions.clear();

	//シミュレーション終了後、安全にアクターを削除する
	for (auto actor : waiting_remove_actors) {

		physics_scene->removeActor(*actor);
		actor->release();
		//二度と参照することがないよう、きっちりnullptrを入れておく
		actor = nullptr;
	}
	for (auto shape : waiting_remove_shapes) {

		shape->release();
		//二度と参照することがないよう、きっちりnullptrを入れておく
		shape = nullptr;
	}
	//全部消えたのですっきりさせる
	waiting_remove_actors.clear();
	waiting_remove_shapes.clear();
}

void Scene::DeleteActor(physx::PxRigidActor* actor)
{
	if (!actor)
		return;
	physics_scene->lockWrite();// PhysX のスレッドをロック

	//以降、Hit処理などに入らないようuserDataを解放して、nullptrにしておく
	auto wp = static_cast<SafeWeakPtr<ObjBase>*>(actor->userData);
	actor->userData = nullptr;
	if (wp)
		delete wp;

	//削除予定アクターに追加登録
	waiting_remove_actors.push_back(actor);

	physics_scene->unlockWrite();// PhysX のスレッドをアンロック
}

void Scene::DeleteShape(physx::PxShape* shape)
{
	if (!shape)
		return;
	auto wp = static_cast<SafeWeakPtr<Collider>*>(shape->userData);
	shape->userData = nullptr;
	if (wp)
		delete wp;

	auto body = shape->getActor();
	if (body)
		body->detachShape(*shape);
	waiting_remove_shapes.push_back(shape);
}


void Scene::Destroy()
{
	ObjBaseWPVec w_vec;
	for (auto& ite : objects)
		w_vec.push_back(ite);
	for (auto& obj : w_vec) {
		if (!obj.lock())
			continue;
		while (auto comp = obj.lock()->GetComponent<Component>()) {
			comp->RemoveThisComponent();
		}
		obj.lock()->Exit();
		obj.lock()->status.status_bit.on(ObjStat::STATUS::REMOVED);
		SceneManager::Object::Destory(obj.lock());
	}
	objects.clear();
	for (auto& ite : leak_objects) {
		try {
			if (ite.lock())
				throw(MemoryLeakException(ite.lock()->name.c_str(), DEFAULT_EXCEPTION_PARAM));
		}
		catch (Exception& ex)
		{
			ex.Show();
		}
	}
	leak_objects.clear();
	if (!physics_scene)
		throw (Exception("PhysXシーンがないンゴ!!ヤバいンゴ!", DEFAULT_EXCEPTION_PARAM));
	Physics();
	int nb_conv = PhysicsManager::GetPhysicsInstance()->getNbConvexMeshes();
	int nb_tringle = PhysicsManager::GetPhysicsInstance()->getNbTriangleMeshes();

}

void Scene::DestroyPhysics()
{
	//PhysXシーンを削除(relese)
	PhysicsManager::ReleaseScene(physics_scene);
	physics_scene = nullptr;
}

void Scene::MoveObjectPtrFromThis(ObjBaseP move_object, SceneP to_where) {
	for (auto pick_obj = objects.begin(); pick_obj != objects.end(); pick_obj++) {
		if (move_object == *pick_obj)
		{
			ObjBase::changed_priority = true;
			(*pick_obj)->scene = to_where;
			to_where->objects.push_back(std::move(*pick_obj));
			objects.erase(pick_obj);
			return;
		}
	}
}
void Scene::DestroyObject(ObjBaseP destroy_obj) {
	if (objects.size() <= 0)
		return;
	if (SceneManager::GetCurrentScene()->IsInSimulation()) {
		SceneManager::GetCurrentScene()->AddFunctionAfterSimulation([this_ = SceneWP(shared_from_this()), obj = ObjBaseWP(destroy_obj)]()
			{if (this_.lock() && obj.lock())
			this_.lock()->DestroyObject(obj.lock());
			});
		return;
	}
	ObjBaseWP obj_w;
	for (auto obj = objects.begin(); obj != objects.end();) {
		obj_w = (*obj);
		if (obj_w.lock() == destroy_obj) {
			obj_w.lock()->transform = nullptr;
			while (obj_w.lock()->GetComponent<Component>()) {
				ComponentWP comp_wp = obj_w.lock()->GetComponent<Component>();
				obj_w.lock()->RemoveComponent(comp_wp.lock());
				try {
					if (comp_wp.lock())
						throw(MemoryLeakException(typeid(*comp_wp.lock().raw_shared().get()).name(), DEFAULT_EXCEPTION_PARAM));
				}
				catch (Exception& ex) {
					ex.Show();
				}
			}
			obj_w.lock()->Exit();
			obj = objects.erase(obj);
			destroy_obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
			destroy_obj.reset();

			if (obj_w.lock())
				leak_objects.push_back(obj_w);
			return;

		}
		obj++;
	}

}
