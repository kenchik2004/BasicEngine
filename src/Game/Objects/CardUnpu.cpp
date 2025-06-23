#include "precompile.h"
#include "CardUnpu.h"


namespace {
	int mytex = -1;

}
namespace RLyeh {

	int CardUnpu::Init()
	{
		if (mytex < 0)
			mytex = LoadGraph(reinterpret_cast<const char*>(u8"data/カード/CardTexture_Unpu.png"));
		my_texture = mytex;
		Super::Init();
		return 0;
	}
	void CardUnpu::Exit()
	{
		DeleteGraph(mytex);
		Super::Exit();
	}
}
