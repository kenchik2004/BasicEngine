//---------------------------------------------------------------------------
//! @file   DontDestroyOnLoadScene.h
//! @brief  シーン遷移後もオブジェクトを保持する専用シーン
//---------------------------------------------------------------------------
#pragma once
#include "Scene.h"
//---------------------------------------------------------------------
//! @class DontDestroyOnLoadScene
//! @brief シーン遷移をまたいで破棄されないオブジェクトを管理するシーン
//---------------------------------------------------------------------
class DontDestroyOnLoadScene :
	public Scene
{
public:
	// 指定オブジェクトをシーン遷移後も破棄されないようにこのシーンへ移動する
	void DontDestroyOnLoad(ObjectP object, SceneP from_where);
};

