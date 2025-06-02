#pragma once
#include "ObjBase.h"
namespace RLyeh {

	class InputManager :
		public Object
	{
		InputManager();
		USING_SUPER(InputManager);
		int Init() override;

	};
}

