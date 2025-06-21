#include "precompile.h"
#include "CardDoman.h"

namespace {
	int mytex = -1;

}
namespace RLyeh {

	int CardDoman::Init()
	{
		if (mytex < 0)
			mytex = LoadGraph(reinterpret_cast<const char*>(u8"data/カード/CardTexture_Doman.png"));
		my_texture = mytex;
		Super::Init();
		return 0;
	}
	void CardDoman::Exit()
	{
		DeleteGraph(mytex);
		Super::Exit();
	}
}
