#pragma once
#include "ObjBase.h"
class DiceD10 :
    public Object
{
public:
	DiceD10(int texture);
	int Init() override;
	void Update() override;
	void DebugDraw() override;
	void LateDebugDraw() override;
	static std::array<Vector3, 10> dice_vectors;
	int handle = -1;
	int selected_number = 0;
};

