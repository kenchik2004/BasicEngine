#include "precompile.h"
#include "System/Scene.h"
#include "System/ObjBase.h"



void Scene::UnLoad()
{
	for (auto& obj : objects) {
		obj->Exit();
	}
}
