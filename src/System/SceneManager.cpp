#include "precompile.h"
#include "System/SceneManager.h"
#include "System/Scene.h"
#include "System/DontDestroyOnLoadScene.h"
#include "System/ObjBase.h"
#include "System/Components/ModelRenderer.h"
#include "algorithm"


ScenePVec SceneManager::scenes = ScenePVec(0);			//!<シーンの配列
ScenePVec SceneManager::another_scenes = ScenePVec(1);	//!<裏シーンの配列
SceneP SceneManager::current_scene = nullptr;			//!<カレントシーン
int SceneManager::debug_box = -1;						//!<デバッグ描画用のボックスモデル

int SceneManager::Init()
{

	//===============================//
	//デバッグ用ボックス(SooS提供)のロード
	debug_box = MV1LoadModel("data/DebugBox/DebugMode/Box.mv1");
	//===============================//

	//裏シーンの配列の0番目は必ずここで作成するシーンが入っている
	auto defaultdontdestroyonload_scene = make_safe_shared<DontDestroyOnLoadScene>();
	defaultdontdestroyonload_scene->Construct<DontDestroyOnLoadScene>();
	another_scenes[0] = defaultdontdestroyonload_scene;

	return 0;
}

void SceneManager::PreUpdate()
{

	for (auto& another_scene : another_scenes)
	{
		if (another_scene->objects.size() > 1 && ObjBase::changed_priority) {

			for (int i = 0; i < another_scene->objects.size() - 1; i++) {
				for (int j = i + 1; j < another_scene->objects.size(); j++) {
					if (another_scene->objects[i]->GetPriority() > another_scene->objects[j]->GetPriority())
						std::swap(another_scene->objects[i], another_scene->objects[j]);

				}
			}
		}
		for (auto& obj : another_scene->objects) {
			if (obj->components.size() > 1 && obj->IsChangedCompPriority()) {

				for (int i = 0; i < obj->components.size() - 1; i++) {
					for (int j = i + 1; j < obj->components.size(); j++) {
						if (obj->components[i]->GetPriority() > obj->components[j]->GetPriority())
							std::swap(obj->components[i], obj->components[j]);

					}
				}
			}
		}
		try {
			another_scene->PreUpdate();

		}
		catch (Exception& ex) {
			ex.Show();
		}
		ObjBaseWPVec objs;
		for (auto& obj : Object::GetArray<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				try {
					obj.lock()->PreUpdate();
				}
				catch (Exception& ex) {
					ex.Show();
				}
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE)) {
							try {
								comp.lock()->PreUpdate();
							}
							catch (Exception& ex) {
								ex.Show();
							}
						}
					});
			}
			});
	}

	//カレントシーンがいないならそちらは何もしない
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
	for (auto& obj : current_scene->GetObjectPtrVec<ObjBase>())
		objs.push_back(obj);

#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PreUpdate();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PreUpdate();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PreUpdate();
			if (!obj.lock())
				continue;
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
#endif
}

void SceneManager::Update()
{

	for (auto& another_scene : another_scenes)
	{
		another_scene->Update();

		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);

		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {

			if (!obj.lock())
				return;

			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->Update();

				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}

				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->Update();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	try {
		current_scene->Update();
	}
	catch (Exception& ex) {
		ex.Show();
	}
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->Update();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->Update();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			try {
			obj.lock()->Update();
			}
			catch (Exception& ex) {
				ex.Show();
			}
			if (!obj.lock())
				continue;
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			for (auto& comp : comps)
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE)) {
						try {
						comp.lock()->Update();
						}
						catch (Exception& ex) {
							ex.Show();
						}
					}
		}
	}
#endif
}

void SceneManager::LateUpdate()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->LateUpdate();
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->LateUpdate();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->LateUpdate();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->LateUpdate();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->LateUpdate();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->LateUpdate();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->LateUpdate();
			if (!obj.lock())
				continue;
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
#endif
}

void SceneManager::PostUpdate()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->PostUpdate();
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->PostUpdate();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->PostUpdate();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PostUpdate();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PostUpdate();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PostUpdate();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PostUpdate();
			if (!obj.lock())
				continue;
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
#endif
}

void SceneManager::PrePhysics()
{

	for (auto& another_scene : another_scenes)
	{
		another_scene->PrePhysics();
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->PrePhysics();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->PrePhysics();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PrePhysics();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PrePhysics();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PrePhysics();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PrePhysics();
			if (!obj.lock())
				continue;
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
#endif
}

void SceneManager::Physics()
{
	for (auto& another_scene : another_scenes)
	{
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->Physics();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->Physics();
					});
			}
			});
		//Physicsだけ、シーンのシミュレーションを最後に行う
		another_scene->Physics();

	}

	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->Physics();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->Physics();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->Physics();
			if (!obj.lock())
				continue;
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
#endif

	//Physicsだけ、シーンのシミュレーションを最後に行う
	current_scene->Physics();
}

void SceneManager::PostPhysics()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->PostPhysics();
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->PostPhysics();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->PostPhysics();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PostPhysics();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PostPhysics();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PostPhysics();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PostPhysics();
			if (!obj.lock())
				continue;
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
#endif
}

void SceneManager::PreDraw()
{
	for (auto& another_scene : another_scenes)
	{

		another_scene->PreDraw();
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->PreDraw();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->PreDraw();
					});
			}
			});
	}
	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->PreDraw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PreDraw();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->PreDraw();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->PreDraw();
			if (!obj.lock())
				continue;
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
#endif
}

void SceneManager::Draw()
{
	for (auto& another_scene : another_scenes)
	{

		another_scene->Draw();
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->Draw();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->Draw();
					});
			}
			});
	}
	if (!current_scene) {
		DrawString(0, 0, "カレントシーンが存在しません", GetColor(255, 0, 0));
		return;
	}
	current_scene->Draw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->Draw();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->Draw();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->Draw();
			if (!obj.lock())
				continue;
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
#endif
}

void SceneManager::LateDraw()
{
	for (auto& another_scene : another_scenes)
	{
		another_scene->LateDraw();
		ObjBaseWPVec objs;
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>())
			objs.push_back(obj);
		std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
			if (!obj.lock())
				return;
			if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
				obj.lock()->LateDraw();
				ComponentWPVec comps;
				for (auto& comp : obj.lock()->components) {
					comps.push_back(comp);
				}
				std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
					if (comp.lock())
						if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
							comp.lock()->LateDraw();
					});
			}
			});
	}

	//カレントシーンがいないならそちらは何もしない
	if (!current_scene)
		return;
	current_scene->LateDraw();
	ObjBaseWPVec objs;
	for (auto& obj : Object::GetArray<ObjBase>())
		objs.push_back(obj);
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->LateDraw();
			ComponentWPVec comps;
			for (auto& comp : obj.lock()->components) {
				comps.push_back(comp);
			}
			std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
				if (comp.lock())
					if (comp.lock()->status.status_bit.is(CompStat::STATUS::ACTIVE))
						comp.lock()->LateDraw();
				});
		}
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		if (obj.lock()->status.status_bit.is(ObjStat::STATUS::ACTIVE)) {
			obj.lock()->LateDraw();
			if (!obj.lock())
				continue;
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
#endif
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
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		obj.lock()->DebugDraw();
		ComponentWPVec comps;
		for (auto& comp : obj.lock()->components) {
			comps.push_back(comp);
		}
		std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
			if (comp.lock())
				comp.lock()->DebugDraw();
			});
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		obj.lock()->DebugDraw();
		if (!obj.lock())
			continue;
		ComponentWPVec comps;
		for (auto& comp : obj.lock()->components) {
			comps.push_back(comp);
		}
		for (auto& comp : comps) {
			if (comp.lock())
				comp.lock()->DebugDraw();
		}
	}
#endif

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
#ifdef FOR_EACH
	std::for_each(objs.begin(), objs.end(), [](ObjBaseWP& obj) {
		if (!obj.lock())
			return;
		obj.lock()->LateDebugDraw();
		ComponentWPVec comps;
		for (auto& comp : obj.lock()->components) {
			comps.push_back(comp);
		}
		std::for_each(comps.begin(), comps.end(), [](ComponentWP& comp) {
			if (comp.lock())
				comp.lock()->LateDebugDraw();
			});
		});
#endif
#ifndef FOR_EACH
	for (auto& obj : objs) {
		if (!obj.lock())
			continue;
		obj.lock()->LateDebugDraw();
		if (!obj.lock())
			continue;
		ComponentWPVec comps;
		for (auto& comp : obj.lock()->components) {
			comps.push_back(comp);
		}
		for (auto& comp : comps) {
			if (comp.lock())
				comp.lock()->LateDebugDraw();
		}
	}
#endif
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
			if (!obj.lock())
				continue;
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

	for (auto& another_scene : another_scenes) {
		for (auto& obj : another_scene->GetObjectPtrVec<ObjBase>()) {

			while (auto comp = obj->GetComponent<Component>()) {
				comp->RemoveThisComponent();
			}
			obj->Exit();
			obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
		}
		another_scene->Exit();
		another_scene->Destroy();
		another_scene->DestroyPhysics();
	}

	//===============================//
	//デバッグ用ボックス(SooS提供)の解放
	MV1DeleteModel(debug_box);
	//===============================//
	ModelRenderer::UnLoad();
	scenes.clear();
	another_scenes.clear();
}

void SceneManager::Object::DontDestroyOnLoad(ObjBaseP obj, SceneP from_where)
{
	SafeStaticCast<DontDestroyOnLoadScene>(another_scenes[0])->DontDestroyOnLoad(obj, from_where);
}
