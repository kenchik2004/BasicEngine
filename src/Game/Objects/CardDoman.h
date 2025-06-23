#pragma once
#include "Game/Objects/CardBase.h"
namespace RLyeh {

	class CardDoman :
		public CardBase
	{
	public:
		USING_SUPER(CardDoman);
		int Init() override;
		void Exit() override;
	};
}

