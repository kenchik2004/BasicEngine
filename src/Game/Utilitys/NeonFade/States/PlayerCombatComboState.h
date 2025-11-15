#pragma once
#include "IState.h"

namespace NeonFade
{
	class Player;
	class Enemy;
	class PlayerCombatComboState :
		public IState
	{
	public:
		PlayerCombatComboState(Player* owner_);
		void OnEnter(IStateMachine* machine) override;
		void OnExit(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnCollisionEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void OnCollisionStay(IStateMachine* machine, const HitInfo& hit_info) override {}
		void OnCollisionExit(IStateMachine* machine, const HitInfo& hit_info) override {}
		void OnTriggerEnter(IStateMachine* machine, const HitInfo& hit_info) override;
		void DebugDraw() override;
	private:
		Player* player = nullptr;
		Enemy* target = nullptr;
		RigidBody* rb = nullptr;
		Animator* animator = nullptr;
		SafeWeakPtr<Collider> hit_box = nullptr;
		bool input_limit = false;
		bool next_avalable = false;
		bool knock_back = false;
	};

}
