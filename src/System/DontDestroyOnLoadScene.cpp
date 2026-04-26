//---------------------------------------------------------------------------
//! @file   DontDestroyOnLoadScene.cpp
//! @brief  DontDestroyOnLoadScene の実装
//---------------------------------------------------------------------------
#include "DontDestroyOnLoadScene.h"


// 指定オブジェクトをこのシーンへ移動し、シーン遷移後も保持させる
void DontDestroyOnLoadScene::DontDestroyOnLoad(ObjectP object, SceneP from_where)
{
	from_where->MoveGameObjectPtrFromThis(object, shared_from_this());
}
