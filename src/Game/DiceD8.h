#pragma once
#include "ObjBase.h"
class DiceD8 :
	public Object
{
public:
	USING_SUPER(DiceD8);
	DiceD8(int texture);
	int Init() override;
	void Update() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	static std::array<Vector3, 8> dice_vectors;
	int handle = -1;
	int selected_number = 0;
};

