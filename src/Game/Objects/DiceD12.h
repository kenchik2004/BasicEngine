#pragma once
#include "Game/Objects/DiceBase.h"

namespace RLyeh {

	class DiceD12 :
		public DiceBase
	{
	public:
		USING_SUPER(DiceD12);
		DiceD12(int texture);
	};

}
