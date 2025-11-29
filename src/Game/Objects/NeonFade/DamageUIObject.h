#pragma once
namespace NeonFade {
	class DamageUIObject :
		public UIObject
	{
	public:
		USING_SUPER(DamageUIObject);
		int Init() override;
		void Update() override;
	private:
		float life_timer = 0.0f;
		static constexpr float LIFE_TIME = 1.0f;
	};
}

