#include "precompile.h"
#include "CardDoman.h"

namespace {
	SafeSharedPtr<Texture> mytex = nullptr;

}
namespace Sample {

	int CardDoman::Init()
	{
		if (!mytex) {
			mytex = TextureManager::CloneByName("card_doman");
		}
		my_texture = mytex;
		Super::Init();
		return 0;
	}
	void CardDoman::Exit()
	{
		mytex.reset();
		Super::Exit();
	}
}
