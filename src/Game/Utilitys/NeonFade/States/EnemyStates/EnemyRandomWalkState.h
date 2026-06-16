#pragma once
#include "Game/Utilitys/NeonFade/States/IState.h"

namespace NeonFade
{
	class Enemy;
	class EnemyRandomWalkState :
		public IState
	{
	public:
		EnemyRandomWalkState(Enemy* owner_enemy_);
		void OnEnter(IStateMachine* machine) override;
		void Update(IStateMachine* machine, float dt) override;
		void OnExit(IStateMachine* machine) override;
		bool CanTransitTo(const std::string& state_name) override;

	private:
		Vector3 next_destination;						//<! 次の目的地
		float walk_speed = 7.5f;						//<! 散歩の速度

		static constexpr float MAX_WALK_TIME = 10.0f;						//<! 散歩の最大時間
		float elapsed_time = 0.0f;							//<! 散歩開始からの経過時間

		Enemy* owner_enemy = nullptr;						//<! オーナーのEnemyオブジェクトへのポインタ
		Animator* animator = nullptr;						//<! オーナーのAnimatorコンポーネントへのポインタ
		RigidBody* rb = nullptr;							//<! オーナーのRigidBodyコンポーネントへのポインタ

	};
}
