#pragma once


USING_PTR(DebugCameraObject);
class DebugCameraObject :public CameraObject
{
public:
	USING_SUPER(DebugCameraObject);
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

	class DebugCameraScene :
		public Scene
	{
	public:
		USING_SUPER(DebugCameraScene);
		int Init() override;

	private:
		DebugCameraObjectWP debug_camera; // デバッグカメラオブジェクトへの弱参照
	};
}

