#pragma once
#include "ObjBase.h"
class DiceD4 :
	public Object
{
public:
	USING_SUPER(DiceD4);
	DiceD4(int texture);
	int Init() override;
	void Update() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	static std::array<Vector3, 4> dice_vectors;
	int handle = -1;
	int selected_number = 0;
};

