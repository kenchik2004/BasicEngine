#pragma once
#include "Game/Objects/Sample/Cards/CardBase.h"
namespace Sample {

class CardFist :
    public CardBase
{
public:
    USING_SUPER(CardFist);
    int Init() override;
    void Exit() override;
};

}
