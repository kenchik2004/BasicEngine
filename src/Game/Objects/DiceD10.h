#pragma once
#include "Game/Objects/DiceBase.h"
namespace RLyeh {

	class DiceD10 :
		public DiceBase
	{
	public:
		USING_SUPER(DiceD10);
		DiceD10(int texture);
	};
}

