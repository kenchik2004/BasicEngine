#pragma once
#include "Game/Managers/LightManager.h"
#include "Game/Utilitys/NeonFade/CatmullRomPath.h"
namespace NeonFade {

	static constexpr int HEADLIGHT_COUNT = 2;


	class FlyingPoliceCarObject :
		public GameObject
	{
	public:
		USING_SUPER(FlyingPoliceCarObject);
		int Init() override;
		void Update() override;
		void Exit() override;

	private:
		std::array<SafeSharedPtr<PointLight>, HEADLIGHT_COUNT> head_lights;
		std::unique_ptr<CatmullRomPath> movement_path;
	};

}
