#pragma once
class EffectPlayer :
	public Component
{
public:
	USING_SUPER(EffectPlayer);
	EffectPlayer() = default;
	EffectPlayer(std::string_view name);
	int Init() override;
	void LateDraw() override;
	void Play();
	void Stop();
	void Load(std::string_view effect_name_);
	void Exit() override;
	void SetSpeed(float speed_) { speed = speed_; }
	float GetSpeed() const { return speed; }
private:
	int effect_handle = -1;
	int playing_handle = -1;
	std::string effect_name;
	float speed = 1.0f;
	bool is_playing = false;
	static std::unordered_map<std::string, int> effect_handle_map;

};

