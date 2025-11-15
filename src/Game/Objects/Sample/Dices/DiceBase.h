#pragma once

namespace Sample {

	class DiceBase :
		public GameObject
	{
	public:
		USING_SUPER(DiceBase);
		DiceBase() = default;
		int Init()override;
		void FetchResult();
		void DebugDraw();
		void LateDebugDraw();
		std::vector<Vector3> dice_vectors;
		int selected_number = 0;
		std::string model_name = "";
	};
}

