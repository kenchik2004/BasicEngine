//---------------------------------------------------------------------------
//! @file   CameraObject.h
//! @brief  カメラ機能を持つゲームオブジェクト
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------
//! @class CameraObject
//! @brief シーン内のカメラとして機能するゲームオブジェクトクラス
//---------------------------------------------------------------------
class CameraObject :
	public GameObject
{
public:
	CameraObject() :GameObject(0U) {}
	USING_SUPER(CameraObject);
	//! @brief 初期化処理
	int Init() override;
	//! @brief 更新処理
	void Update() override;
	CameraWP camera; // カメラコンポーネント
};

#if 0
class DebugCameraObject :
	public GameObject
{
public:
	USING_SUPER(DebugCameraObject);
	int Init() override;
	void PostPhysics() override;
	DebugCameraWP camera; // カメラコンポーネント
	CameraWP main_camera; // メインカメラ
};
#endif
