#include "System/Scene.h"
#include "System/Objects/Object.h"
#include "System/Components/RigidBody.h"
#include "System/Components/Collider.h"
#include <algorithm>



void Scene::Physics()
{
#ifndef PACKAGE_BUILD
	if (!physics_scene)
		throw Exception("PhysXシーンがないンゴ!!ヤバいンゴ!", DEFAULT_EXCEPTION_PARAM);
#endif

	//シミュレーション
	in_simulation = true;
	//精度向上のため、サブステップで4回に分けてシミュレーションを行う
	double single_step_time = Time::FixedDeltaTime() * physics_timescale / 4.0;
	for (u32 i = 0; i < 4; i++) {
		physics_scene->simulate(single_step_time);
		//シミュレーションを待つ
		while (!physics_scene->fetchResults(true)) {}
	}


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
	auto wp = static_cast<SafeWeakPtr<Object>*>(actor->userData);
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
	ObjectWPVec w_vec;
	for (auto& ite : objects)
		w_vec.push_back(ite);
	for (auto& obj : w_vec) {
		if (!obj)
			continue;
		DestroyGameObject(obj.lock());
	}
	objects.clear();
	for (auto& ite : leak_objects) {
		try {
			if (ite) {
#ifdef PACKAGE_BUILD
				std::quick_exit(0);
#endif
				throw(MemoryLeakException(ite->name.c_str(), DEFAULT_EXCEPTION_PARAM));
			}
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

void Scene::MoveGameObjectPtrFromThis(ObjectP move_object, SceneP to_where) {

	//どちらかがnullptrの場合は論外。何もしない
	if (!move_object || !to_where)
		return;

	//コライダーコンポーネントとRigidBodyコンポーネントは
	//オブジェクトのシーンに依存しているので、
	//先に登録を解除しておく
	auto cols = move_object->GetComponents<Collider>();
	for (auto& col : cols) {
		//ここでシェイプの削除を行っている
		col->Exit();
	}

	auto rb = move_object->GetComponent<RigidBody>();
	//ここでアクターの削除を行っている
	if (rb)
		rb->Exit();

	//大事!!ここでは、終了処理を呼んでいるが、コンポーネント自体は削除していないため、
	//持っているデータ(velocityやradiusなど)は消えていないのでこの後再利用する

	//objectsからの削除
	for (auto pick_obj = objects.begin(); pick_obj != objects.end();) {
		if (move_object == *pick_obj)
		{
			(*pick_obj)->scene = to_where;
			//譲渡先では新規作成扱いなので、dirtyとしても登録
			to_where->dirty_priority_objects.push_back(*pick_obj);
			to_where->objects.push_back(std::move(*pick_obj));
			//こちらではもう所有しません
			objects.erase(pick_obj);
			break;
		}
		pick_obj++;
	}
	//dirty_priority_objからの消去
	// 
	// 新規作成したオブジェクトの場合
	// こっちに残ったものがobjectsに挿入され、
	// 多重管理が起きてしまう

	for (auto pick_obj = dirty_priority_objects.begin(); pick_obj != dirty_priority_objects.end();) {

		if (move_object == *pick_obj) {
			dirty_priority_objects.erase(pick_obj);
			break;
		}
		pick_obj++;
	}
	//これでオブジェクト所有権を他のシーンへ譲渡できたので、
	//先ほどのコライダーたちの初期化処理を呼んでやることで破棄せず再登録できる

	//ここでアクターの登録を行っている
	//必ず先にRigidBodyを初期化(Colliderでbodyを使用するため)
	if (rb)
		rb->Init();

	for (auto& col : cols) {
		//ここでシェイプの作成を行っている
		col->Init();
	}


}

void Scene::RegisterActiveCamera(CameraWP camera)
{
	if (!camera)
		return;
	auto it = std::find(active_cameras.begin(), active_cameras.end(), camera);
	if (it == active_cameras.end()) {
		active_cameras.push_back(camera);
	}
}

void Scene::UnregisterActiveCamera(CameraWP camera)
{
	if (!camera)
		return;
	auto it = std::find(active_cameras.begin(), active_cameras.end(), camera);
	if (it != active_cameras.end()) {
		active_cameras.erase(it);
	}
}

void Scene::SyncGameObjectsPriority()
{

	if (dirty_priority_objects.empty()) return;

	// 1) dirty_priority_objects を優先度順に並べ替える
	std::sort(dirty_priority_objects.begin(), dirty_priority_objects.end(),
		[](ObjectP a, ObjectP b) { return a->GetPriority() < b->GetPriority(); });


	// 2) まとめて objects へ挿入
	for (ObjectP& obj : dirty_priority_objects)
	{
		if (obj->status.status_bit.is(ObjStat::STATUS::REMOVED))
			continue;
		// 2‑1) 古い場所を消す（同じポインタ重複を防ぐ）
		auto cur = std::find(objects.begin(), objects.end(), obj);
		if (cur != objects.end()) {
			int index = std::distance(objects.begin(), cur);
			objects.erase(cur);
		}

	}
	for (ObjectP& obj : dirty_priority_objects)
	{
		// 2‑2) 優先度に合った場所を二分探索で探す
		auto pos = FindInsertPositionByPriority(obj->GetPriority());

		objects.insert(objects.begin() + pos, obj);  // ここに差し込む
	}
	dirty_priority_objects.clear();
	//Time::ResetTime();
}

size_t Scene::FindInsertPositionByPriority(unsigned int priority)
{

	// 優先度に合った場所を二分探索で探す
	// upper_bound は「指定した値より大きい最初の位置」を返す
	// つまり、同じ優先度のものがあればその後ろに挿入される
	auto pos = std::upper_bound(
		objects.begin(),
		objects.end(),
		priority,
		[](const unsigned int lhs, const ObjectP& rhs) {
			return lhs < rhs->GetPriority();
		}
	);
	return std::distance(objects.begin(), pos);
}

void Scene::DestroyGameObject(ObjectP destroy_obj) {
	if (objects.size() <= 0)
		return;
	if (IsInSimulation()) {
		AddFunctionAfterSimulation([this_ = SceneWP(shared_from_this()), obj = ObjectWP(destroy_obj)]()
			{if (this_ && obj)
			this_->DestroyGameObject(obj.lock());
			});
		return;
	}
	ObjectWP obj_w;
	if (auto obj = std::find(dirty_priority_objects.begin(), dirty_priority_objects.end(), destroy_obj); obj != dirty_priority_objects.end())
	{
		dirty_priority_objects.erase(obj);
	}
	for (auto obj = objects.begin(); obj != objects.end();) {
		obj_w = (*obj);
		if (obj_w.lock() == destroy_obj) {
			obj_w->transform = nullptr;
			while (obj_w->GetComponent<Component>()) {
				ComponentWP comp_wp = obj_w.lock()->GetComponent<Component>();
				obj_w->RemoveComponent(comp_wp.lock());
				try {
					if (comp_wp)
						throw(MemoryLeakException(typeid(*comp_wp.lock().raw_shared().get()).name(), DEFAULT_EXCEPTION_PARAM));
				}
				catch (Exception& ex) {
					ex.Show();
				}
			}
			obj_w->Exit();
			obj = objects.erase(obj);
			destroy_obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
			destroy_obj.reset();

			if (obj_w)
				leak_objects.push_back(obj_w);
			return;

		}
		obj++;
	}

}
#if 0
void Scene::DestroyGameObject(ObjectP& destroy_obj) {
	if (objects.size() <= 0)
		return;
	if (IsInSimulation()) {
		AddFunctionAfterSimulation([this_ = SceneWP(shared_from_this()), obj = ObjectWP(destroy_obj)]()
			{if (this_ && obj)
			this_->DestroyGameObject(obj.lock());
			});
		return;
	}
	if (auto obj = std::find(dirty_priority_objects.begin(), dirty_priority_objects.end(), destroy_obj); obj != dirty_priority_objects.end())
	{
		dirty_priority_objects.erase(obj);
	}
	if (auto obj = std::find(objects.begin(), objects.end(), destroy_obj); obj != objects.end();) {
		destroy_obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
		destroy_obj.reset();
		(*obj).reset();
	}
}

}

#endif