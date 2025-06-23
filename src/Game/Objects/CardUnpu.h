#pragma once
#include "CardBase.h"
namespace RLyeh {

	class CardUnpu :
		public CardBase
	{
	public:
		USING_SUPER(CardUnpu);
		int Init() override;
		void Exit() override;
	};

}
