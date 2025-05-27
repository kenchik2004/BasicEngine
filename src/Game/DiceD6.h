#pragma once
#include "ObjBase.h"
class DiceD6 :
	public Object
{
public:
	USING_SUPER(DiceD6);
	DiceD6(int texture);
	int Init() override;
	void Update() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	static std::array<Vector3, 6> dice_vectors;
	int handle = -1;
	int selected_number = 0;
};

