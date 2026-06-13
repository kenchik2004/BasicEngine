#include "OptionExit.h"

namespace NeonFade {

	int OptionExit::Init() {
		int ret = Super::Init();
		SetText(u8"EXIT");
		on_decide = []() { SceneManager::CloseApplication(); };
		return ret;
	}

}
