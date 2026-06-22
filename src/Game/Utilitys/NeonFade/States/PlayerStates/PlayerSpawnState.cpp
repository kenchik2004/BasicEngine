#include "PlayerSpawnState.h"
#include "Game/Objects/NeonFade/Player.h"

namespace NeonFade {
	PlayerSpawnState::PlayerSpawnState(Player* owner_player_)
		:IState(static_cast<GameObject*>(owner_player_))
	{
		owner_player = owner_player_;
		animator = owner_player->animator.lock().get();
		std::function<bool()> default_exit = [this]() {
			return elapsed_time >= SPAWN_DURATION;
			};
		RegisterChangeRequest("fall", default_exit, 0);
	}
	void PlayerSpawnState::OnEnter(IStateMachine* machine)
	{
		elapsed_time = 0.0f;
		animator->Play("player_dive", true);
		
		owner_player->transform->SetAxisY({ 0, 1, 1 });
		trail_effect =  SceneManager::Object::Create<GameObject>(u8"effect_dive");
		trail_effect->transform->position = owner_player->transform->position + Vector3(0, 0.5f, 0);
		trail_effect->transform->scale = { 3.0f, 3.0f, 3.0f };
		auto eff_player = trail_effect->AddComponent<EffectPlayer>(u8"data/FX/Simple_Trail.efkefc");
		eff_player->Play(true);
	}
	void PlayerSpawnState::Update(IStateMachine* machine, float dt)
	{
		elapsed_time += dt;
		trail_effect->transform->position=owner_player->transform->position + Vector3(0, 2.5f, 0);
	}
	void PlayerSpawnState::OnExit(IStateMachine* machine)
	{
		owner_player->transform->SetAxisY({ 0, 1, 0 });
		SceneManager::Object::Destroy(trail_effect.lock());

	}
}