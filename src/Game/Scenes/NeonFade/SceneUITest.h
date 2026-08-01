#pragma once

namespace NeonFade {

	class SceneUITest :
		public Scene
	{
	public:
		USING_SUPER(SceneUITest);
		void Load() override;
		int Init() override;
		void Update() override;
		void UnLoad() override;
	};

}
