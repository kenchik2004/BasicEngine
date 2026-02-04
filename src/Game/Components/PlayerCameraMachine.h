#pragma once
namespace NeonFade {
	class Player;
	class PlayerCameraMachine :
		public Component
	{
	public:
		enum CAMERA_MODE {
			MANIPULATE,
			CINEMATIC,
		};
		USING_SUPER(Component);
		int Init() override;
		void PreDraw() override;
		void Update() override;
		void ShakeCamera(float intensity_, float duration_);
		void SetTarget(SafeWeakPtr<Player> target_) { target = target_; }
		void SetCameraMode(CAMERA_MODE mode_, bool transition_enable = true) { mode = mode_; if (transition_enable)transition_timer = transition_time_max; }
		CAMERA_MODE GetCameraMode() const { return mode; }
		void SetCinematicOffset(const Vector3& offset_) { cinematic_offset = offset_; }
		void SetTransitionTime(float time_) { transition_time_max = time_; }
		float camera_distance_max = 30.0f;
	private:
		CAMERA_MODE mode;
		float transition_timer = 0.0f;
		float transition_time_max = 0.5f;
		SafeWeakPtr<Player> target;
		Vector3 shake_offset = { 0,0,0 };
		float shake_intensity = 0.0f;
		float shake_duration = 0.0f;
		bool reverse_y = false;

		Vector2 camera_rot = { 0,0 };
		Vector3 cinematic_offset = { 0,0,-1 };
		float camera_distance = 20.0f;

	};
}

