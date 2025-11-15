#pragma once

USING_PTR(AudioListener);
class AudioListener final :
	public Component
{
	bool is_current_listener = false;
public:
	USING_SUPER(AudioListener);
	void Construct() override;
	int Init() override;
	void Update() override;
	void SetCurrentListener();
	SafeSharedPtr<AudioListener> GetCurrentListener();

};

