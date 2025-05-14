#pragma once
#include "ObjBase.h"
class DiceD8 :
	public Object
{
public:
	DiceD8(int texture);
	int Init() override;
	void Update() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	static std::array<Vector3, 8> dice_vectors;
	int handle = -1;
	int selected_number = 0;
};

