#include "CardFist.h"
namespace {
	SafeSharedPtr<Texture> mytex = nullptr;

}
namespace Sample {

	int CardFist::Init()
	{
		if (!mytex) {
			mytex = TextureManager::CloneByName("card_fist");
		}
		my_texture = mytex;
		Super::Init();
		return 0;
	}
	void CardFist::Exit()
	{
		mytex.reset();
		Super::Exit();
	}
}
