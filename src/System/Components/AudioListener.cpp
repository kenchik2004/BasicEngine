#include "precompile.h"
#include "AudioListener.h"




int AudioListener::Init()
{
	if (!GetCurrentListener())
		SetCurrentListener();
	return 0;
}

void AudioListener::PreDraw()
{
	if (is_current_listener)
		Set3DSoundListenerPosAndFrontPosAndUpVec(cast(owner->transform->position), cast(owner->transform->position + owner->transform->AxisZ()), cast(owner->transform->AxisY()));

}

void AudioListener::SetCurrentListener()
{
	auto current_listener = SceneManager::GetCurrentScene()->GetCurrentAudioListener();
	if (current_listener.lock())
		SafeStaticCast<AudioListener>(current_listener.lock())->is_current_listener = false;
	is_current_listener = true;
	SceneManager::GetCurrentScene()->SetCurrentAudioListener(shared_from_this());
}

SafeSharedPtr<AudioListener> AudioListener::GetCurrentListener()
{
	return SafeStaticCast<AudioListener>(SceneManager::GetCurrentScene()->GetCurrentAudioListener().lock());
}
