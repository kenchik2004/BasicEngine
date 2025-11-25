#pragma once

class PointLight;
namespace NeonFade
{
	class PoliceCar :
		public GameObject
	{
	public:
		USING_SUPER(GameObject);
		int Init() override;
		void Update() override;
		void Exit() override;
		void OnTriggerEnter(const HitInfo& hit_info)override;
		void OnTriggerExit(const HitInfo& hit_info) override;
		void SetPointOnpath(const float& t_) { t = t_; }

	private:
		static inline bool player_rideon = false;
		float t = 0.0f;
		RigidBodyWP rb = nullptr;
		AudioPlayerWP audio_player = nullptr;
		SafeSharedPtr<PointLight> moving_light_red = nullptr;
		SafeSharedPtr<PointLight> moving_light_blue = nullptr;
		std::vector<SafeWeakPtr<Object>> rideon_objs;
	};
}

