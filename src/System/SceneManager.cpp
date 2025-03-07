#include "precompile.h"
#include "System/SceneManager.h"
#include "System/Scene.h"
#include "System/ObjBase.h"
#include "System/Components/ModelRenderer.h"


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
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PreUpdate();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PreUpdate();
		}
	}
}

void SceneManager::Update()
{
	if (!current_scene)
		return;
	current_scene->Update();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->Update();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->Update();
		}
	}
}

void SceneManager::LateUpdate()
{
	if (!current_scene)
		return;
	current_scene->LateUpdate();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->LateUpdate();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->LateUpdate();
		}
	}
}

void SceneManager::PostUpdate()
{
	if (!current_scene)
		return;
	current_scene->PostUpdate();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PostUpdate();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PostUpdate();
		}
	}
}

void SceneManager::PrePhysics()
{
	if (!current_scene)
		return;
	current_scene->PrePhysics();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PrePhysics();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PrePhysics();
		}
	}
}

void SceneManager::Physics()
{
	if (!current_scene)
		return;
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->Physics();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->Physics();
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
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PostPhysics();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PostPhysics();
		}
	}
}

void SceneManager::PreDraw()
{
	if (!current_scene)
		return;
	current_scene->PreDraw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PreDraw();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PreDraw();
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
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			obj.lock()->Draw();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->Draw();
		}
	}
}

void SceneManager::LateDraw()
{
	if (!current_scene)
		return;
	current_scene->LateDraw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::DRAW)) {
			obj.lock()->LateDraw();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->LateDraw();
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

	SetUseLighting(false);
	SetLightEnable(false);
	//===============================//
	current_scene->DebugDraw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		obj.lock()->DebugDraw();
		ComponentWPVec comps;
		for (auto& comp : obj.lock()->components) {
			comps.push_back(comp);
		}
		for (auto& comp : comps)
			if (comp.lock())
				comp.lock()->DebugDraw();

	}
	SetUseLighting(true);
	SetLightEnable(true);
}

void SceneManager::LateDebugDraw()
{
	if (!current_scene) return;
	current_scene->LateDebugDraw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		obj.lock()->LateDebugDraw();
		ComponentWPVec comps;
		for (auto& comp : obj.lock()->components) {
			comps.push_back(comp);
		}
		for (auto& comp : comps)
			if (comp.lock())
				comp.lock()->LateDebugDraw();

	}
}

void SceneManager::PostDraw()
{
	if (!current_scene)
		return;
	current_scene->PostDraw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PostDraw();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PostDraw();
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
		current_scene->Destroy();
		current_scene->DestroyPhysics();
		for (auto ite = scenes.begin(); ite != scenes.end(); ite++) {
			if ((*ite) == current_scene)
			{
				scenes.erase(ite);
				current_scene = nullptr;
				break;
			}
		}
	}
	for (auto& scene : scenes) {
		scene->UnLoad();
		scene->Destroy();
		scene->DestroyPhysics();
	}

	//===============================//
	//デバッグ用ボックス(SooS提供)の解放
	MV1DeleteModel(debug_box);
	//===============================//
	ModelRenderer::UnLoad();
	scenes.clear();
}


