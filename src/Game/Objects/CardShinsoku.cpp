#include "precompile.h"
#include "CardShinsoku.h"
namespace {
	int mytex = -1;

}
namespace RLyeh {

	int CardShinsoku::Init()
	{
		if (mytex < 0)
			mytex = LoadGraph(reinterpret_cast<const char*>(u8"data/カード/CardTexture_Shinsoku.png"));
		my_texture = mytex;
		Super::Init();
		return 0;
	}
	void CardShinsoku::Exit()
	{
		DeleteGraph(mytex);
		Super::Exit();
	}
}
