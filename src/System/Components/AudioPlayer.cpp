#include "precompile.h"
#include "AudioPlayer.h"
#include "RigidBody.h"
void AudioPlayer::SetAudio(std::string_view name, std::string_view new_name)
{
	std::string name_(name);
	audio = AudioManager::CloneByName(name, new_name);
	if (!audio)
		return;
	default_frequency = GetFrequencySoundMem(audio->handle);
}

void AudioPlayer::PreDraw()
{
	auto audio_listener = SceneManager::GetCurrentScene()->GetCurrentAudioListener();
	if (audio && audio_listener.lock()) {
		auto listener = audio_listener.lock()->owner.lock();
		auto rb = listener->GetComponent<RigidBody>();
		Vector3 vec_v_o = Vector3();
		Vector3 vec_v_s = Vector3();
		if (rb) {
			vec_v_o = rb->velocity;
		}
		rb = owner.lock()->GetComponent<RigidBody>();
		if (rb) {
			vec_v_s = rb->velocity;
		}
		auto vec_pos = owner.lock()->transform->position - listener->transform->position;
		float v_o = vec_v_o.magnitude() * -vec_v_o.getNormalized().dot(vec_pos.getNormalized());
		float v_s = vec_v_s.magnitude();
		float f_;
		if (v_s < 0.01f)
			v_s = 0.01f; //ゼロ除算を防ぐための最小値
		f_ = ((340.0f - v_o) / (340.0f - v_s));
		int frequency = (int)(default_frequency * f_ * pitch_rate);
		if (frequency < 100)
			frequency = 0;
		if (frequency > 100000)
			frequency = 44100;
		SetFrequencySoundMem(frequency, audio->handle);
		Set3DPositionSoundMem(cast(owner->transform->position), audio->handle);
		Set3DRadiusSoundMem(radius, audio->handle);
	}
}

