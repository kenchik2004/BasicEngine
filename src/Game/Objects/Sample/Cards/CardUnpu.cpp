#include "precompile.h"
#include "CardUnpu.h"


namespace {
	SafeSharedPtr<Texture> mytex = nullptr;

}
namespace Sample {

	int CardUnpu::Init()
	{
		if (!mytex) {
			mytex = TextureManager::CloneByName("card_unpu");
		}
		my_texture = mytex;
		Super::Init();
		return 0;
	}
	void CardUnpu::Exit()
	{
		mytex.reset();
		Super::Exit();
	}
}
