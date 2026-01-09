#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"
class LightManager;
class PointLight;
namespace NeonFade {
	class Player;
	class PlayerSmashChargeState :
		public IState
	{
	public:
		PlayerSmashChargeState(Player* player_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
	private:
		SafeSharedPtr<Texture> charge_emi_tex = nullptr;
		SafeSharedPtr<Texture> def_emi_tex = nullptr;
		Player* owner_player;
		RigidBody* rb;
		Animator* animator;
		float charge_timer = 0.0f;
		SafeWeakPtr<GameObject> charge_effect = nullptr;
		LightManager* light_manager = nullptr;
		std::vector<SafeSharedPtr<PointLight>> smash_lights;
		static constexpr float MAX_CHARGE_TIME = 1.5f;
	};
}

