#pragma once
#include "Game/Objects/Sample/Cards/CardBase.h"
namespace Sample {

	class CardShinsoku :
		public CardBase
	{
	public:
		USING_SUPER(CardShinsoku);
		int Init() override;
		void Exit() override;
	};

}
