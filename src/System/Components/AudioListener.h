#pragma once
#include "System/Component.h"
class AudioListener final :
	public Component
{
	bool is_current_listener = false;
public:
	int Init() override;
	void PreDraw() override;
	void SetCurrentListener();
	SafeSharedPtr<AudioListener> GetCurrentListener();
};

