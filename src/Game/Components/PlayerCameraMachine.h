#pragma once
namespace NeonFade {
	class Player;
	class PlayerCameraMachine :
		public Component
	{
	public:
		USING_SUPER(Component);
		int Init() override;
		void PreDraw() override;
		void Update() override;
		void ShakeCamera(float intensity_, float duration_);
		void SetTarget(SafeWeakPtr<Player> target_) { target = target_; }
		float camera_distance_max = 30.0f;
	private:
		SafeWeakPtr<Player> target;
		Vector3 shake_offset = { 0,0,0 };
		float shake_intensity = 0.0f;
		float shake_duration = 0.0f;
		bool reverse_y = false;

		Vector2 camera_rot = { 0,0 };
		float camera_distance = 20.0f;

	};
}

