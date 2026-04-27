//---------------------------------------------------------------------------
//! @file   GameObjectWithLifeTime.h
//! @brief  ライフタイム付きゲームオブジェクト（一定時間後に自動消滅）
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------
//! @class GameObjectWithLifeTime
//! @brief 生存時間が設定されたゲームオブジェクト
//---------------------------------------------------------------------
class GameObjectWithLifeTime :
	public GameObject
{
public:
	USING_SUPER(GameObjectWithLifeTime);
	GameObjectWithLifeTime(const float lifetime_) :GameObject(5000), lifetime(lifetime_) {};
	//! @brief 初期化処理
	int Init() override;
	//! @brief 更新処理
	void Update() override;
	const float lifetime; //!< 生存時間（秒）
	float lifetimer = 0.0f; //!< 経過タイマー
	Vector3 move_dir = Vector3(0, 0, 0); //!< 移動方向ベクトル
};
