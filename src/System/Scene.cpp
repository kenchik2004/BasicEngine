#include "precompile.h"
#include "System/Scene.h"
#include "System/ObjBase.h"




void Scene::Destroy()
{
	for (auto& obj : objects) {
		while (auto comp = obj->GetComponent<Component>()) {
			comp->RemoveThisComponent();
		}
		obj->Exit();
		obj->status.status_bit.on(ObjStat::STATUS::REMOVED);
	}
}
