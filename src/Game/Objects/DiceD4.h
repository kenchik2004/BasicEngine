#pragma once
#include "Game/Objects/DiceBase.h"
namespace RLyeh {

	class DiceD4 :
		public DiceBase
	{
	public:
		USING_SUPER(DiceD4);
		DiceD4(int texture);
	};

}
