#include "precompile.h"
#include "DontDestroyOnLoadScene.h"
#include "Game/SceneSample.h"


void DontDestroyOnLoadScene::DontDestroyOnLoad(ObjBaseP object, SceneP from_where)
{
	from_where->MoveObjectPtrFromThis(object, shared_from_this());
}
