#pragma once
#include "ObjBase.h"
class DiceD20 :
	public Object
{
public:
	USING_SUPER(DiceD20);
	DiceD20(int texture);
	int Init() override;
	void Update() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	static std::array<Vector3, 20> dice_vectors;
	int handle = -1;
	int selected_number = 0;
};

