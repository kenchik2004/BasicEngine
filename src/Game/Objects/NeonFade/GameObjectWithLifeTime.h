#pragma once

class GameObjectWithLifeTime :
	public GameObject
{
public:
	USING_SUPER(GameObjectWithLifeTime);
	GameObjectWithLifeTime(const float lifetime_) :GameObject(5000), lifetime(lifetime_) {};
	int Init() override;
	void Update() override;
	const float lifetime;
	float lifetimer = 0.0f;
};

