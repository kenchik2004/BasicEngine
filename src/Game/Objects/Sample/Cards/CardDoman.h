#pragma once
#include "Game/Objects/Sample/Cards/CardBase.h"
namespace Sample {

	class CardDoman :
		public CardBase
	{
	public:
		USING_SUPER(CardDoman);
		int Init() override;
		void Exit() override;
	};
}

