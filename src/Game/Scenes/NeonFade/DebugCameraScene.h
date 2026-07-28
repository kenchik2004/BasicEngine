// DebugCameraScene.h
#pragma once

/// @brief デバッグ用カメラオブジェクトのクラス
/// @details 開発中の空間移動や時間操作を行うためのカメラ
USING_PTR(DebugCameraObject);
class DebugCameraObject :public CameraObject
{
public:
	USING_SUPER(DebugCameraObject);
	/// @brief 物理演算後の更新処理を行う
	void PostPhysics() override;
private:
	//Quaternionで直接制御するとZ軸回転が入ったり、角度制限が設けづらいので、オイラー角で回転を制御する
	Vector2 rot_euler = Vector2(0, 0);
	bool is_active = true; // DebugCameraが有効かどうか
	float rot_speed = 1.0f; // 回転速度
	float move_speed = 15.0f; // 移動速度
	static constexpr float MOVE_SPEED_MAX = 100.0f; // 移動速度の最大値
	static constexpr float MOVE_SPEED_MIN = 0.1f; // 移動速度の最小値
};

namespace NeonFade {

	/// @brief デバッグカメラを管理するシーンクラス
	/// @details デバッグ用カメラの生成や初期化を担う
	class DebugCameraScene :
		public Scene
	{
	public:
		USING_SUPER(DebugCameraScene);
		/// @brief シーンの初期化処理を行う
		/// @return 初期化の成否を示す整数値
		int Init() override;

	private:
		DebugCameraObjectWP debug_camera; // デバッグカメラオブジェクトへの弱参照
	};
}