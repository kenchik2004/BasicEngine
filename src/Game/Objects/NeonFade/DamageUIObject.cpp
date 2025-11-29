#include "DamageUIObject.h"

namespace NeonFade {
	int DamageUIObject::Init()
	{
		return 0;
	}
	void DamageUIObject::Update()
	{
		float dt = Time::UnscaledDeltaTime();
		life_timer += dt;
		transform->position.y += 10.0f * dt;
		if (life_timer >= LIFE_TIME) {
			SceneManager::Object::Destroy(shared_from_this());
		}
	}
}
