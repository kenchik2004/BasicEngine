﻿#pragma once
#include "Game/Objects/DiceBase.h"

namespace RLyeh {

	class DiceD20 :
		public DiceBase
	{
	public:
		USING_SUPER(DiceD20);
		DiceD20(int texture);

	};
}

