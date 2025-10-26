#pragma once
#include "Game/Objects/Sample/Cards/CardBase.h"
namespace Sample {

	class CardUnpu :
		public CardBase
	{
	public:
		USING_SUPER(CardUnpu);
		int Init() override;
		void Exit() override;
	};

}
