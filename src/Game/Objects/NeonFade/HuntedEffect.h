#pragma once

namespace NeonFade {
	class HuntedEffect :
		public UIObject
	{
	public:
		USING_SUPER(UIObject);
		int Init() override;
		void Update() override;

	private:
		float lifetime = 0.0f;
		static constexpr float LIFE_TIME = 30.0f;
	};

}
