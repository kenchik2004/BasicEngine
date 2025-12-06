#include "GameObjectWithLifeTime.h"

int GameObjectWithLifeTime::Init()
{
	lifetimer = lifetime;
	return Super::Init();
}

void GameObjectWithLifeTime::Update()
{
	transform->position += move_dir * Time::UnscaledDeltaTime();
	lifetimer -= Time::UnscaledDeltaTime();
	if (lifetimer <= 0.0f)
		SceneManager::Object::Destroy(shared_from_this());
}
