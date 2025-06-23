#include "precompile.h"
#include "CardFist.h"
namespace {
	int mytex = -1;

}
namespace RLyeh {

	int CardFist::Init()
	{
		if (mytex < 0)
			mytex = LoadGraph(reinterpret_cast<const char*>(u8"data/カード/CardTexture_Cobushi.png"));
		my_texture = mytex;
		Super::Init();
		return 0;
	}
	void CardFist::Exit()
	{
		DeleteGraph(mytex);
		Super::Exit();
	}
}
