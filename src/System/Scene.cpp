#include "precompile.h"
#include "System/Scene.h"
#include "System/ObjBase.h"




void Scene::Physics()
{
	if (!physics_scene)
		throw Exception("PhysXシーンがないンゴ!!ヤバいンゴ!", DEFAULT_EXCEPTION_PARAM);

	//シミュレーション
	in_simulation = true;
	physics_scene->simulate(Time::FixedDeltaTime());
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
	auto wp = static_cast<std::weak_ptr<ObjBase>*>(actor->userData);
	actor->userData = nullptr;
	if (wp->lock())
		delete wp;
	////アクターが持っているシェイプ情報を全部取得
	//physx::PxU32 num_shapes = actor->getNbShapes();
	//std::vector<physx::PxShape*> shapes(num_shapes);
	//actor->getShapes(shapes.data(), num_shapes);

	////全てのシェイプに対し、削除予定のフラグを立てておく
	//for (auto shape : shapes) {
	//	physx::PxFilterData filter_data;
	//	filter_data = { 0,0,0,0 }; // 削除対象マーク(フィルターで全ビット0は、当たり判定無効化)
	//	shape->setSimulationFilterData(filter_data);
	//}
	//削除予定アクターに追加登録
	waiting_remove_actors.push_back(actor);

	physics_scene->unlockWrite();// PhysX のスレッドをアンロック
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
		throw (Exception("PhysXシーンがないンゴ!!ヤバいンゴ!", DEFAULT_EXCEPTION_PARAM));
	Physics();



}

void Scene::DestroyPhysics()
{
	//PhysXシーンを削除(relese)
	PhysicsManager::ReleaseScene(physics_scene);
	physics_scene = nullptr;
}
