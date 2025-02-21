#include "precompile.h"
#include "System/SceneManager.h"
#include "System/Scene.h"
#include "System/ObjBase.h"


ScenePVec SceneManager::scenes = ScenePVec(0);
SceneP SceneManager::current_scene = nullptr;
int SceneManager::debug_box = -1;

int SceneManager::Init()
{

	//===============================//
	//デバッグ用ボックス(SooS提供)のロード
	debug_box = MV1LoadModel("data/DebugBox/DebugMode/Box.mv1");
	//===============================//
	return 0;
}

void SceneManager::PreUpdate()
{
	if (!current_scene)
		return;
	if (current_scene->objects.size() > 1 && ObjBase::changed_priority) {

		for (int i = 0; i < current_scene->objects.size() - 1; i++) {
			for (int j = i + 1; j < current_scene->objects.size(); j++) {
				if (current_scene->objects[i]->GetPriority() > current_scene->objects[j]->GetPriority())
					std::swap(current_scene->objects[i], current_scene->objects[j]);

			}
		}
		ObjBase::changed_priority = false;
	}
	for (auto& obj : current_scene->objects) {
		if (obj->components.size() > 1 && obj->IsChangedCompPriority()) {

			for (int i = 0; i < obj->components.size() - 1; i++) {
				for (int j = i + 1; j < obj->components.size(); j++) {
					if (obj->components[i]->GetPriority() > obj->components[j]->GetPriority())
						std::swap(obj->components[i], obj->components[j]);

				}
			}
		}
	}

	current_scene->PreUpdate();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PreUpdate();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PreUpdate();
			}

		}
	}
}

void SceneManager::Update()
{
	if (!current_scene)
		return;
	current_scene->Update();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->Update();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->Update();
			}
		}
	}
}

void SceneManager::LateUpdate()
{
	if (!current_scene)
		return;
	current_scene->LateUpdate();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->LateUpdate();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->LateUpdate();
			}
		}
	}
}

void SceneManager::PostUpdate()
{
	if (!current_scene)
		return;
	current_scene->PostUpdate();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostUpdate();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PostUpdate();
			}
		}
	}
}

void SceneManager::PrePhysics()
{
	if (!current_scene)
		return;
	current_scene->PrePhysics();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PrePhysics();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PrePhysics();
			}
		}
	}
}

void SceneManager::Physics()
{
	if (!current_scene)
		return;
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->Physics();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->Physics();
			}
		}
	}

	//Physicsだけ、シーンのシミュレーションを最後に行う
	current_scene->Physics();
}

void SceneManager::PostPhysics()
{
	if (!current_scene)
		return;
	current_scene->PostPhysics();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostPhysics();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PostPhysics();
			}
		}
	}
}

void SceneManager::PreDraw()
{
	if (!current_scene)
		return;
	current_scene->PreDraw();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			obj->PreDraw();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PreDraw();
			}
		}
	}
}

void SceneManager::Draw()
{
	if (!current_scene) {
		DrawString(0, 0, "カレントシーンが存在しません", GetColor(255, 0, 0));
		return;
	}
	current_scene->Draw();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			obj->Draw();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->Draw();
			}
		}
	}
}

void SceneManager::LateDraw()
{
	if (!current_scene)
		return;
	current_scene->LateDraw();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			obj->LateDraw();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->LateDraw();
			}
		}
	}
}

void SceneManager::DebugDraw()
{
	if (!current_scene) {
		DrawString(0, 0, "カレントシーンが存在しません", GetColor(255, 0, 0));
		return;
	}
	//===============================//
	//デバッグ用ボックス(SooS提供)の描画
	MV1SetPosition(debug_box, VGet(0, 0, 0));
	//MV1SetScale(debug_box, VGet(10, 10, 10));
	MV1DrawModel(debug_box);

	//===============================//
	current_scene->DebugDraw();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		obj->DebugDraw();

		for (auto& comp : obj->components) {
			comp->DebugDraw();
		}
	}
}

void SceneManager::LateDebugDraw()
{
	if (!current_scene) return;	
	current_scene->LateDebugDraw();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		obj->LateDebugDraw();

		for (auto& comp : obj->components) {
			comp->LateDebugDraw();
		}
	}
}

void SceneManager::PostDraw()
{
	if (!current_scene)
		return;
	current_scene->PostDraw();
	for (auto& obj : Object::GetArray<ObjBase>()) {
		if (obj->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj->PostDraw();

			for (auto& comp : obj->components) {
				if (comp->status.status_bit.is(CompStat::STATUS::ACTIVE))
					comp->PostDraw();
			}
		}
	}
}

void SceneManager::Exit()
{
	if (current_scene) {
		for (auto& obj : Object::GetArray<ObjBase>()) {

			while (auto comp = obj->GetComponent<Component>()) {
				comp->RemoveThisComponent();
			}
			obj->Exit();
			obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
		}
		current_scene->Exit();
	}
	current_scene = nullptr;
	for (auto& scene : scenes) {
		scene->UnLoad();
	}

	//===============================//
	//デバッグ用ボックス(SooS提供)の解放
	MV1DeleteModel(debug_box);
	//===============================//
	scenes.clear();
}


