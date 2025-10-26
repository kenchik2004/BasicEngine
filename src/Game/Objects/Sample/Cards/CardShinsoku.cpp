#include "precompile.h"
#include "CardShinsoku.h"
namespace {
	SafeSharedPtr<Texture> mytex = nullptr;

}
namespace Sample {

	int CardShinsoku::Init()
	{
		if (!mytex) {
			mytex = TextureManager::CloneByName("card_shinsoku");
		}
		my_texture = mytex;		
		Super::Init();
		return 0;
	}
	void CardShinsoku::Exit()
	{
		mytex.reset();
		Super::Exit();
	}
}
